RSpec.describe(EnterpriseScriptService::ServiceChannel) do
  let(:writer) { instance_double(IO, "writer") }
  let(:reader) { instance_double(IO, "reader") }
  let(:service_channel) do
    EnterpriseScriptService::ServiceChannel.new(writer, reader)
  end

  it "forwards #write to the writer" do
    allow(writer).to receive(:write).and_return(writer)
    expect(service_channel.write("hello")).to eq(nil)
    expect(writer).to have_received(:write).with("hello")
  end

  it "rescues EPIPE from the writer" do
    expect(writer).to receive(:write).and_raise(Errno::EPIPE)
    expect(service_channel.write("hello")).to eq(nil)
  end

  it "forwards #read to the reader" do
    expect(reader).to receive(:read).and_return("hello")
    expect(service_channel.read).to eq("hello")
  end
end
