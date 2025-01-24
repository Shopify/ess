require("msgpack")
require("open3")

RSpec.describe(EnterpriseScriptService) do
  it "evaluates a simple script" do
    result = EnterpriseScriptService.run(
      input: {result: [26803196617, 0.475]},
      sources: [
        ["stdout", "@stdout_buffer = 'hello'"],
        ["foo", "@output = @input[:result]"],
      ],
      timeout: 1000,
    )
    expect(result.success?).to be(true)
    expect(result.output).to eq([26803196617, 0.475])
    expect(result.stdout).to eq("hello")
  end

  it "round trips binary strings" do
    result = EnterpriseScriptService.run(
      input: "hello".force_encoding(Encoding::BINARY),
      sources: [
        ["foo", "@output = @input"],
      ],
      timeout: 1000,
    )
    expect(result.success?).to be(true)
    expect(result.output).to eq("hello")
  end

  it "exposes metrics" do
    result = EnterpriseScriptService.run(
      input: {result: [26803196617, 0.475]},
      sources: [
        ["stdout", "@stdout_buffer = 'hello'"],
        ["foo", "@output = @input[:result]"],
      ],
      timeout: 1000,
    )

    expect(result.measurements.keys).to eq([
      :in, :mem, :init, :sandbox,
      :decode, :inject, :lib, :compile,
      :eval, :out,
    ])
  end

  it "exposes stat" do
    result = EnterpriseScriptService.run(
      input: {result: [26803196617, 0.475]},
      sources: [
        ["stdout", "@stdout_buffer = 'hello'"],
        ["foo", "@output = @input[:result]"],
      ],
      timeout: 1000,
    )
    expect(result.stat.instructions).to eq(8)
  end

  SCRIPT_SETUP_INSTRUCTION_COUNT = 17
  INSTRUCTION_COUNT_PER_LOOP = 13 #For .times {}

  def expected_instructions(loops)
    SCRIPT_SETUP_INSTRUCTION_COUNT + INSTRUCTION_COUNT_PER_LOOP * loops
  end

  def max_loops(instruction_quota)
    ((instruction_quota - SCRIPT_SETUP_INSTRUCTION_COUNT) / INSTRUCTION_COUNT_PER_LOOP).floor
  end

  it "checks that the instruction quota defaults to 100000 when no limit is given to the ESS" do
    loops = max_loops(100000)

    result = EnterpriseScriptService.run(
      input: {result: [26803196617, 0.475]},
      sources: [
        ["stdout", "#{loops}.times {}"],
      ],
      timeout: 1000,
    )

    expect(result.success?).to be(true)
    expect(result.stat.instructions).to eq(expected_instructions(loops))

    result_with_error = EnterpriseScriptService.run(
      input: {result: [26803196617, 0.475]},
      sources: [
        ["stdout", "#{loops + 1}.times {}"],
      ],
      timeout: 1000,
    )

    expect(result_with_error.success?).to be(false)
    expect(result_with_error.output).to be_nil
    expect(result_with_error.errors).to include(a_kind_of(EnterpriseScriptService::EngineInstructionQuotaError))
  end

  it "checks that a given instruction quota is respected" do
    given_quota = rand(1..9999999)
    loops = max_loops(given_quota)

    result = EnterpriseScriptService.run(
      input: {result: [26803196617, 0.475]},
      sources: [
        ["stdout", "#{loops}.times {}"],
      ],
      timeout: 1000,
      instruction_quota: given_quota,
    )

    expect(result.success?).to be(true)
    expect(result.stat.instructions).to eq(expected_instructions(loops))

    result_with_error = EnterpriseScriptService.run(
      input: {result: [26803196617, 0.475]},
      sources: [
        ["stdout", "#{loops + 1}.times {}"],
      ],
      timeout: 1000,
      instruction_quota: given_quota,
    )

    expect(result_with_error.success?).to be(false)
    expect(result_with_error.output).to be_nil
    expect(result_with_error.errors).to include(a_kind_of(EnterpriseScriptService::EngineInstructionQuotaError))
  end

  it "checks that a given instruction quota is respected from a given source index" do
    quota = 15000
    result = EnterpriseScriptService.run(
      input: {result: [26803196617, 0.475]},
      sources: [
        ["ignore", "1000.times {}"], #if we count this one, we'd blow the 15k quota
        ["count", "1000.times {}"],
      ],
      timeout: 1000,
      instruction_quota: quota,
      instruction_quota_start: 1,
    )

    expect(result.success?).to be(true)
    expect(result.stat.total_instructions).to be > quota
  end

  it "supports symbols stat" do
    result = EnterpriseScriptService.run(
      input: {result: {value: 0.475}},
      sources: [
        ["stdout", "@stdout_buffer = 'hello'"],
        ["foo", "@output = @input[:result]"],
      ],
      timeout: 1000,
    )
    expect(result.output).to eq({value: 0.475})
  end

  it "reports syntax errors" do
    result = EnterpriseScriptService.run(
      input: "Yay!",
      sources: [["syntax_error.rb", "end"]],
      timeout: 1,
    )
    expect(result.success?).to be(false)
    expect(result.output).to eq(nil)
    expect(result.errors).to have_attributes(length: 1)

    error = result.errors[0]
    expect(error).to be_an(EnterpriseScriptService::EngineSyntaxError)
    expect(error.message).to eq("syntax_error.rb:1:3: syntax error, unexpected keyword_end")
    expect(error.filename).to eq("syntax_error.rb")
    expect(error.line_number).to eq(1)
    expect(error.column).to eq(3)
  end

  it "reports raised exception" do
    result = EnterpriseScriptService.run(
      input: "Yay!",
      sources: [["raise.rb", <<-SOURCE]],
        def foo
          raise("foo")
        end

        foo
      SOURCE
      timeout: 1,
    )
    expect(result.success?).to be(false)
    expect(result.output).to eq(nil)
    expect(result.errors).to have_attributes(length: 1)

    error = result.errors[0]
    expect(error).to be_an(EnterpriseScriptService::EngineRuntimeError)
    expect(error.message).to eq("foo")
    expect(error.guest_backtrace).to eq([
      "raise.rb:2:in Object.foo",
      "raise.rb:5",
    ])
  end

  it "reports metrics on raised exception" do
    result = EnterpriseScriptService.run(
      input: "Yay!",
      sources: [["foo",'raise "foobar"']],
      timeout: 1,
    )
    expect(result.success?).to be(false)
    expect(result.measurements.keys).to eq([
      :in, :mem, :init, :sandbox,
      :decode, :inject, :lib, :compile,
      :eval, :out,
    ])
  end

  it "reports an engine runtime fatal error on a bad input" do
    result = EnterpriseScriptService.run(input: '"Yay!"', sources: "", timeout: 1)
    expect(result.success?).to be(false)
    expect(result.errors).to have_attributes(length: 1)

    error = result.errors[0]
    expect(error).to be_an(EnterpriseScriptService::EngineAbnormalExitError)
    expect(error.code).to eq(2)
  end

  it "reports an unknown type error on a bad output" do
    result = EnterpriseScriptService.run(input: "Yay!", sources: [["test", "@output = Class"]], timeout: 1)
    expect(result.success?).to be(false)
    expect(result.errors).to have_attributes(length: 1)

    error = result.errors[0]
    expect(error).to be_an(EnterpriseScriptService::UnknownTypeError)
  end

  it "does work with a simple payload" do
    service_path ||= begin
      base_path = Pathname.new(__dir__).parent
      base_path.join("bin/enterprise_script_service".freeze).to_s
    end

    Open3.popen3("#{service_path} < tests/data/data.mp") do |stdin, stdout, stderr, thread|
      expect(thread.value.exitstatus).to be(0)
    end
  end

  it "includes the stdout_buffer if it raises" do
    result = EnterpriseScriptService.run(
      input: {result: [26803196617, 0.475]},
      sources: [
        ["stdout", "@stdout_buffer = 'hello'"],
        ["foo", "raise \"Ouch!\""],
      ],
      timeout: 1000,
    )
    expect(result.success?).to be(false)
    expect(result.output).to eq(nil)
    expect(result.stdout).to eq("hello")
  end

  it "accepts large inputs" do
    result = EnterpriseScriptService.run(
      input: "a" * 180 * 1024, # 180KiB
      sources: [],
      timeout: 1000,
    )
    expect(result.success?).to be(true)
  end

  it "fails gracefully when input is too large" do
    result = EnterpriseScriptService.run(
      input: "a" * 8 * 1024 * 1024, # 8MiB, total limit
      sources: [],
      timeout: 1000,
    )
    expect(result.success?).to be(false)
    expect(result.errors).to eq([EnterpriseScriptService::EngineMemoryQuotaError.new])
  end

  it "roundtrips input with null bytes" do
    assert_roundtrip_input("foo\0bar")
  end

  it "roundtrips emojis" do
    assert_roundtrip_input("😅")
  end

  it "handles generational_gc being false" do
    result = EnterpriseScriptService.run(
      input: {result: [26803196617, 0.475]},
      sources: [
        ["stdout", "@stdout_buffer = 'hello'"],
        ["foo", "@output = @input[:result]"],
      ],
      timeout: 1000,
      generational_gc: false
    )
    expect(result.success?).to be(true)
    expect(result.output).to eq([26803196617, 0.475])
    expect(result.stdout).to eq("hello")
  end

  it "calls the binary with GC when generational gc is unset" do
    expect(EnterpriseScriptService::ServiceProcess).to receive(:new).with(
      Pathname.new(__dir__).parent.join("bin/enterprise_script_service").to_s,
      anything,
      anything,
      anything
    )
    runner = double("runner")
    allow(runner).to receive(:run)
    allow(EnterpriseScriptService::Runner).to receive(:new).and_return(runner)

    EnterpriseScriptService.run(
      input: {result: [26803196617, 0.475]},
      sources: [
        ["stdout", "@stdout_buffer = 'hello'"],
        ["foo", "@output = @input[:result]"],
      ],
      timeout: 1000,
    )
  end

  it "calls the binary with GC when generational gc is true" do
    expect(EnterpriseScriptService::ServiceProcess).to receive(:new).with(
      Pathname.new(__dir__).parent.join("bin/enterprise_script_service").to_s,
      anything,
      anything,
      anything
    )
    runner = double("runner")
    allow(runner).to receive(:run)
    allow(EnterpriseScriptService::Runner).to receive(:new).and_return(runner)

    EnterpriseScriptService.run(
      input: {result: [26803196617, 0.475]},
      sources: [
        ["stdout", "@stdout_buffer = 'hello'"],
        ["foo", "@output = @input[:result]"],
      ],
      timeout: 1000,
      generational_gc: true
    )
  end

  it "calls the binary with no gen GC when generational gc is false" do
    expect(EnterpriseScriptService::ServiceProcess).to receive(:new).with(
      Pathname.new(__dir__).parent.join("bin/enterprise_script_service_no_gen_gc").to_s,
      anything,
      anything,
      anything
    )
    runner = double("runner")
    allow(runner).to receive(:run)
    allow(EnterpriseScriptService::Runner).to receive(:new).and_return(runner)

    EnterpriseScriptService.run(
      input: {result: [26803196617, 0.475]},
      sources: [
        ["stdout", "@stdout_buffer = 'hello'"],
        ["foo", "@output = @input[:result]"],
      ],
      timeout: 1000,
      generational_gc: false
    )
  end

  private

  def assert_roundtrip_input(input)
    result = EnterpriseScriptService.run(
      input: input,
      sources: [
        ["foo", "@output = @input"],
      ],
      timeout: 1000,
    )
    expect(result.success?).to be(true)
    expect(result.output).to eq(input)
  end
end
