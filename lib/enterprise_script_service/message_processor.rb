module EnterpriseScriptService
  class MessageProcessor
    def initialize
      @measurements = {}
      @stat = EnterpriseScriptService::Stat::Null
      @errors = []
      @output = nil
      @stdout = ""
    end

    def process_all(channel)
      unpacker = EnterpriseScriptService::Protocol.packer_factory.unpacker(channel)
      begin
        unpacker.each do |raw_message|
          read(raw_message)
        end
      rescue EOFError
        signal_truncation
      end
    end

    def signal_error(error)
      @errors << error
    end

    def signal_truncation
      signal_error(EnterpriseScriptService::EngineTruncationError.new)
    end

    def signal_signaled(signal)
      error = case signal
      when 31
        EnterpriseScriptService::EngineIllegalSyscallError.new
      else
        EnterpriseScriptService::EngineSignaledError.new(signal: signal)
      end

      signal_error(error)
    end

    def signal_abnormal_exit(code)
      error = case code
      when 8
        EnterpriseScriptService::ArithmeticOverflowError.new
      when 9
        EnterpriseScriptService::UnknownTypeError.new
      when 16
        EnterpriseScriptService::EngineMemoryQuotaError.new
      when 17
        EnterpriseScriptService::EngineInstructionQuotaError.new
      when 19
        EnterpriseScriptService::EngineTypeError.new
      else
        EnterpriseScriptService::EngineAbnormalExitError.new(code: code)
      end

      signal_error(error)
    end

    def to_result
      EnterpriseScriptService::Result.new(
        output: @output,
        stdout: @stdout,
        stat: @stat,
        errors: @errors,
        measurements: @measurements,
      )
    end

    private

    def read(raw_message)
      type, data = raw_message
      case type
      when :output then read_output(data)
      when :error then read_error(data)
      when :measurement then read_measurement(data)
      when :stat then read_stat(data)
      end
    end

    def read_output(data)
      @output = data[:extracted]
      @stdout = data[:stdout]
    end

    def read_error(data)
      @errors <<
        case data[:__type]
        when :runtime
          EngineRuntimeError.new(
            data[:message],
            guest_backtrace: data[:backtrace],
          )
        when :syntax
          EngineSyntaxError.new(
            data[:message],
            filename: data[:filename],
            line_number: data[:line_number],
            column: data[:column],
          )
        when :unknown_type
          EngineUnknownTypeError.new(type: data[:type])
        when :unknown_ext
          EngineUnknownExtError.new(type: data[:type])
        else
          EngineInternalError.new("unknown error: #{data}")
        end
    end

    def read_measurement(data)
      name, microseconds = *data
      if @measurements.has_key?(name) 
        @measurements[name] += microseconds
      else
        @measurements[name] = microseconds
      end
    end

    def read_stat(data)
      @errors << EnterpriseScriptService::DuplicateMessageError.new(
        "duplicate stat message",
      ) unless @stat == EnterpriseScriptService::Stat::Null
      @stat = EnterpriseScriptService::Stat.new(data)
    end
  end
end
