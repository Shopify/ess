RSpec.describe(EnterpriseScriptService::MessageProcessor) do
  let(:message_processor) { EnterpriseScriptService::MessageProcessor.new }

  let(:packer) { EnterpriseScriptService::Protocol.packer_factory.packer }

  it "processes an output message" do
    io = StringIO.new(packer.pack([
      :output,
      extracted: {dog: "fine"},
      stdout: "hello",
    ]))

    message_processor.process_all(io)
    expect(message_processor.to_result).to eq(
      EnterpriseScriptService::Result.new(
        output: {dog: "fine"},
        stdout: "hello",
        stat: EnterpriseScriptService::Stat::Null,
        errors: [],
        measurements: {},
      ),
    )
  end

  it "processes a runtime error message" do
    io = StringIO.new(packer.pack([
      :error,
      __type: :runtime,
      message: "oops",
      backtrace: ["oops.rb:1:5"],
    ]))

    message_processor.process_all(io)
    result = message_processor.to_result
    error = EnterpriseScriptService::EngineRuntimeError.new(
      "oops", guest_backtrace: ["oops.rb:1:5"],
    )
    expect(result).to eq(
      EnterpriseScriptService::Result.new(
        output: nil,
        stdout: "",
        stat: EnterpriseScriptService::Stat::Null,
        errors: [error],
        measurements: {},
      ),
    )
    expect(result.errors.first.guest_backtrace).to eq(error.guest_backtrace)
  end

  it "processes a runtime error message" do
    io = StringIO.new(packer.pack([
      :error,
      __type: :syntax,
      message: "unexpected keyword end",
      filename: "syntax_error.rb",
      line_number: 3,
      column: 40,
    ]))

    message_processor.process_all(io)
    result = message_processor.to_result
    error = EnterpriseScriptService::EngineSyntaxError.new(
      "unexpected keyword end",
      filename: "syntax_error.rb",
      line_number: 3,
      column: 40,
    )
    expect(result).to eq(
      EnterpriseScriptService::Result.new(
        output: nil,
        stdout: "",
        stat: EnterpriseScriptService::Stat::Null,
        errors: [error],
        measurements: {},
      ),
    )
    expect(result.errors.first.filename).to eq(error.filename)
    expect(result.errors.first.line_number).to eq(error.line_number)
    expect(result.errors.first.column).to eq(error.column)
  end

  it "processes an unknown type error" do
    io = StringIO.new(packer.pack([
      :error,
      __type: :unknown_type,
      type: 9,
    ]))

    message_processor.process_all(io)
    result = message_processor.to_result
    error = EnterpriseScriptService::EngineUnknownTypeError.new(type: 9)
    expect(result).to eq(
      EnterpriseScriptService::Result.new(
        output: nil,
        stdout: "",
        stat: EnterpriseScriptService::Stat::Null,
        errors: [error],
        measurements: {},
      ),
    )
  end

  it "processes an unknown ext error" do
    io = StringIO.new(packer.pack([
      :error,
      __type: :unknown_ext,
      type: 1,
    ]))

    message_processor.process_all(io)
    result = message_processor.to_result
    error = EnterpriseScriptService::EngineUnknownExtError.new(type: 1)
    expect(result).to eq(
      EnterpriseScriptService::Result.new(
        output: nil,
        stdout: "",
        stat: EnterpriseScriptService::Stat::Null,
        errors: [error],
        measurements: {},
      ),
    )
  end

  it "processes an unknown error" do
    io = StringIO.new(packer.pack([
      :error,
      __type: :lol,
      message: "yolo",
    ]))

    message_processor.process_all(io)
    result = message_processor.to_result
    error = EnterpriseScriptService::EngineInternalError.new(
      %(unknown error: #{{__type: :lol, message: "yolo"}})
    )
    expect(result).to eq(
      EnterpriseScriptService::Result.new(
        output: nil,
        stdout: "",
        stat: EnterpriseScriptService::Stat::Null,
        errors: [error],
        measurements: {},
      ),
    )
  end

  it "reads a measurement" do
    io = StringIO.new(packer.pack([
      :measurement,
      [:transmogrification, 14],
    ]))

    message_processor.process_all(io)
    result = message_processor.to_result
    expect(result).to eq(
      EnterpriseScriptService::Result.new(
        output: nil,
        stdout: "",
        stat: EnterpriseScriptService::Stat::Null,
        errors: [],
        measurements: {transmogrification: 14},
      ),
    )
  end

  it "adds up measurements" do
    io = StringIO.new(packer.pack([
      :measurement,
      [:transmogrification, 14],
    ]))
    io << packer.pack([
      :measurement,
      [:transmogrification, 12],
    ])
    io.rewind

    message_processor.process_all(io)
    result = message_processor.to_result
    expect(result).to eq(
      EnterpriseScriptService::Result.new(
        output: nil,
        stdout: "",
        stat: EnterpriseScriptService::Stat::Null,
        errors: [],
        measurements: {transmogrification: 26},
      ),
    )
  end

  it "reads stats" do
    io = StringIO.new(packer.pack([
      :stat,
      instructions: 12345,
      memory: 2334,
    ]))

    message_processor.process_all(io)
    result = message_processor.to_result
    expect(result).to eq(
      EnterpriseScriptService::Result.new(
        output: nil,
        stdout: "",
        stat: EnterpriseScriptService::Stat.new(
          instructions: 12345,
          memory: 2334,
        ),
        errors: [],
        measurements: {},
      ),
    )
  end

  describe "#signal_abnormal_exit" do
    it "returns an UnknownTypeError when called with code 9" do
      message_processor.signal_abnormal_exit(9)
      errors = message_processor.to_result.errors

      expect(errors).to eq([
        EnterpriseScriptService::UnknownTypeError.new,
      ])
    end

    it "returns an EngineMemoryQuotaError when called with code 16" do
      message_processor.signal_abnormal_exit(16)
      errors = message_processor.to_result.errors

      expect(errors).to eq([
        EnterpriseScriptService::EngineMemoryQuotaError.new,
      ])
    end

    it "returns an EngineInstructionQuotaError when called with code 17" do
      message_processor.signal_abnormal_exit(17)
      errors = message_processor.to_result.errors

      expect(errors).to eq([
        EnterpriseScriptService::EngineInstructionQuotaError.new,
      ])
    end

    it "returns an EngineAbnormalExitError for an unspecific code" do
      message_processor.signal_abnormal_exit(123)
      errors = message_processor.to_result.errors

      expect(errors).to eq([
        EnterpriseScriptService::EngineAbnormalExitError.new(code: 123),
      ])
    end
  end

  describe "#signal_signaled" do
    it "returns an EngineIllegalSyscallError when called with signal 31" do
      message_processor.signal_signaled(31)
      errors = message_processor.to_result.errors

      expect(errors).to eq([
        EnterpriseScriptService::EngineIllegalSyscallError.new,
      ])
      expect(errors[0]).to be_kind_of(EnterpriseScriptService::EngineSignaledError)
      expect(errors[0].signal).to eq(31)
    end

    it "returns an EngineSignaledError when called with another signal" do
      message_processor.signal_signaled(40)
      errors = message_processor.to_result.errors

      expect(errors).to eq([
        EnterpriseScriptService::EngineSignaledError.new(signal: 40),
      ])
    end
  end
end
