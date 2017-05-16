require 'timeout'

module EnterpriseScriptService
  class Runner
    attr_reader(:timeout, :service_process, :message_processor_factory)

    def initialize(timeout:, service_process:, message_processor_factory:)
      @timeout = timeout
      @service_process = service_process
      @message_processor_factory = message_processor_factory
    end

    def run(*data)
      message_processor = message_processor_factory.new

      begin
        code = service_process.open do |channel|
          Timeout.timeout(timeout) do
            data.each { |datum| channel.write(datum) } 
            message_processor.process_all(channel)
          end
        end

        if code > 255
          message_processor.signal_signaled(code - 255)
        elsif code != 0
          message_processor.signal_abnormal_exit(code)
        end
      rescue Timeout::Error
        message_processor.signal_error(
          EnterpriseScriptService::EngineTimeQuotaError.new(quota: timeout),
        )
      end

      message_processor.to_result
    end
  end
end
