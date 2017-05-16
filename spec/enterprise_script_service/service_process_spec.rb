RSpec.describe(EnterpriseScriptService::ServiceProcess) do
  let(:pid) { rand(1 << 16) }
  let(:service_path) { "bin/my-subprocess" }
  let(:pipe_pairs) { [] }

  let(:spawner) do
    spawner = instance_double(EnterpriseScriptService::Spawner)
    allow(spawner).to receive(:spawn).and_return(pid)
    allow(spawner).to receive(:wait).and_return(0)
    spawner
  end

  let(:service_process) do
    EnterpriseScriptService::ServiceProcess.new(service_path, spawner, 100000, 2)
  end

  it "creates a channel to communicate with the subprocess" do
    in_reader = nil
    out_writer = nil
    expect(spawner).to receive(:spawn) do |path, **options|
      expect(path).to eq(service_path)
      in_reader = options[:in].dup
      expect(in_reader).to be_an(IO)
      out_writer = options[:out].dup
      expect(out_writer).to be_an(IO)
      expect(options[:unsetenv_others]).to eq(true)
    end

    service_process.open do |service_channel|
      service_channel.write("hello")
      expect(in_reader.readpartial(1000)).to eq("hello")
      in_reader.close

      out_writer.write("bye")
      out_writer.close
      expect(service_channel.read).to eq("bye")
    end
  end

  it "open passes arguments to process" do
    expect(spawner)
      .to receive(:spawn).once.with(instance_of(String), "-i", 100000.to_s, "-C", 2.to_s, instance_of(Hash))
    service_process.open do |c|
    end
  end

  it "optimistically tries to wait on the child without killing" do
    expect(spawner)
      .to receive(:wait).once.with(pid, Process::WNOHANG).and_return(0)
    expect(spawner).to receive(:wait).never
    expect(service_process.open { }).to eq(0)
  end

  it "optimistically tries to wait on the child without killing when raising" do
    expect(spawner)
      .to receive(:wait).once.with(pid, Process::WNOHANG).and_return(0)
    expect(spawner).to receive(:wait).never
    expect { service_process.open { raise("oops") } }.to raise_error("oops")
  end

  it "kills the child process if it has not terminated" do
    expect(spawner)
      .to receive(:wait).once.with(pid, Process::WNOHANG).and_return(nil)
    expect(spawner).to receive(:kill).once.with(9, pid)
    expect(spawner)
      .to receive(:wait).once.with(pid).and_return(0)
    expect(service_process.open { }).to eq(0)
  end

  it "kills the child process if it has not terminated when raising" do
    expect(spawner)
      .to receive(:wait).once.with(pid, Process::WNOHANG).and_return(nil)
    expect(spawner).to receive(:kill).once.with(9, pid)
    expect(spawner)
      .to receive(:wait).once.with(pid).and_return(0)
    expect { service_process.open { raise("oops") } }.to raise_error("oops")
  end

  it "returns -1 when killing failed" do
    expect(spawner)
      .to receive(:wait).once.with(pid, Process::WNOHANG).and_return(nil)
    expect(spawner).to receive(:kill).once.with(9, pid).and_raise(Errno::ESRCH)
    expect(service_process.open { }).to eq(-1)
  end
end
