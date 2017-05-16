RSpec.describe(EnterpriseScriptService::Runner) do
  let(:result) { instance_double(EnterpriseScriptService::Result) }

  let(:message_processor) do
    message_processor = instance_double(EnterpriseScriptService::MessageProcessor)
    allow(message_processor).to receive(:to_result).and_return(result)
    message_processor
  end

  let(:message_processor_factory) do
    factory = instance_double(Class)
    allow(factory).to receive(:new).and_return(message_processor)
    factory
  end

  let(:service_process) do
    instance_double(EnterpriseScriptService::ServiceProcess)
  end

  context "#run terminates before time is out" do
    let(:channel) do
      channel = instance_double(EnterpriseScriptService::ServiceChannel)
      expect(channel).to receive(:write).once.with("hello")
      channel
    end

    let(:runner) do
      EnterpriseScriptService::Runner.new(
        timeout: 10,
        service_process: service_process,
        message_processor_factory: message_processor_factory,
      )
    end

    it "handles a successful run" do
      expect(service_process).to receive(:open) do |&block|
        block.call(channel)
        0
      end

      expect(message_processor).to receive(:process_all).once.with(channel)
      expect(runner.run("hello")).to be(result)
    end

    it "handles a failed run" do
      expect(service_process).to receive(:open) do |&block|
        block.call(channel)
        1
      end

      expect(message_processor).to receive(:process_all).once.with(channel)
      expect(message_processor).to receive(:signal_abnormal_exit).once.with(1)
      expect(runner.run("hello")).to be(result)
    end

    it "handles a signaled run" do
      expect(service_process).to receive(:open) do |&block|
        block.call(channel)
        286
      end

      expect(message_processor).to receive(:process_all).once.with(channel)
      expect(message_processor).to receive(:signal_signaled).once.with(31)
      expect(runner.run("hello")).to be(result)
    end
  end

  context "#run times out" do
    let(:channel) do
      instance_double(EnterpriseScriptService::ServiceChannel)
    end

    before do
      expect(service_process).to receive(:open) do |&block|
        block.call(channel)
        0
      end
    end

    let(:runner) do
      EnterpriseScriptService::Runner.new(
        timeout: 0.01,
        service_process: service_process,
        message_processor_factory: message_processor_factory,
      )
    end

    it "times out on writes" do
      expect(channel).to receive(:write) { sleep(1) }
      allow(message_processor).to receive(:process_all)

      expect(message_processor).to receive(:signal_error).once.with(
        EnterpriseScriptService::EngineTimeQuotaError.new(quota: 0.01),
      )
      expect(runner.run("hello")).to be(result)
    end

    it "times out on reads" do
      allow(channel).to receive(:write).with("hello")
      allow(message_processor).to receive(:process_all) { sleep(1) }

      expect(message_processor).to receive(:signal_error).once.with(
        EnterpriseScriptService::EngineTimeQuotaError.new(quota: 0.01),
      )
      expect(runner.run("hello")).to be(result)
    end
  end
end
