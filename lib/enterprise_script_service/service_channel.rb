module EnterpriseScriptService
  class ServiceChannel
    attr_reader(:in_writer, :out_reader)

    def initialize(in_writer, out_reader)
      @in_writer = in_writer
      @out_reader = out_reader
    end

    def write(buffer)
      in_writer.write(buffer)
      nil
    rescue Errno::EPIPE
    end

    def read(*args)
      out_reader.read(*args)
    end
  end
end
