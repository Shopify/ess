module EnterpriseScriptService
  # one to rule them all
  EngineError = Class.new(StandardError)

  # abstract super types
  EngineScriptError = Class.new(EngineError)
  EngineQuotaError = Class.new(EngineError)
  EngineInternalError = Class.new(EngineError)

  # Script Errors
  class EngineSyntaxError < EngineScriptError
    attr_reader :filename, :line_number, :column

    def initialize(message, filename:, line_number:, column:)
      super(message)
      @filename = filename
      @line_number = line_number
      @column = column
    end

    def inspect
      %(#<#{self.class.name}: #{message}>)
    end

    def message
      %(#{filename}:#{line_number}:#{column}: #{super})
    end
  end

  class EngineRuntimeError < EngineScriptError
    attr_reader :guest_backtrace

    def initialize(message, guest_backtrace:)
      super(message)
      @guest_backtrace = guest_backtrace
    end

    def inspect
      "#<#{self.class.name}: #{message}: #{guest_backtrace.inspect}>"
    end
  end

  # Quota Errors
  EngineMemoryQuotaError = Class.new(EngineQuotaError)
  EngineInstructionQuotaError = Class.new(EngineQuotaError)
  EngineStackExhaustedError = Class.new(EngineQuotaError) # unused?

  class EngineTimeQuotaError < EngineQuotaError
    def initialize(quota:) 
      super("Timed out after #{quota}")
    end
  end

  # Internal Errors
  EngineTypeError = Class.new(EngineInternalError)
  UnknownTypeError = Class.new(EngineInternalError)
  EngineTruncationError = Class.new(EngineInternalError)
  ArithmeticOverflowError = Class.new(EngineInternalError)

  class EngineUnknownTypeError < EngineInternalError
    attr_reader :type

    def initialize(type:)
      @type = type
      super("unknown type #{type}")
    end
  end

  class EngineUnknownExtError < EngineInternalError
    attr_reader :type

    def initialize(type:)
      @type = type
      super("unknown ext #{type}")
    end
  end

  class EngineAbnormalExitError < EngineInternalError
    attr_reader :code

    def initialize(code:)
      @code = code
      super("engine exited with code #{code}")
    end
  end

  class EngineSignaledError < EngineInternalError
    attr_reader :signal

    def initialize(signal:)
      @signal = signal
      super("engine terminated with signal #{signal}")
    end
  end

  class EngineIllegalSyscallError < EngineSignaledError
    def initialize
      super(signal: 31)
    end
  end
end
