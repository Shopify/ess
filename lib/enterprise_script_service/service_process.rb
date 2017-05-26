module EnterpriseScriptService
  class ServiceProcess
    attr_reader(:path, :spawner, :instruction_quota, :instruction_quota_start, :memory_quota)

    def initialize(path, spawner, instruction_quota, instruction_quota_start, memory_quota)
      @path = path
      @spawner = spawner
      @instruction_quota = instruction_quota
      @instruction_quota_start = instruction_quota_start
      @memory_quota = memory_quota
    end

    def open
      in_reader, in_writer = IO.pipe
      out_reader, out_writer = IO.pipe

      pid = spawner.spawn(
        path,
        "-i", instruction_quota.to_s,
        "-C", instruction_quota_start.to_s,
        "-m", memory_quota.to_s,
        in: in_reader,
        out: out_writer,
        unsetenv_others: true,
      )

      in_reader.close
      out_writer.close

      in_writer.binmode
      out_reader.binmode

      begin
        yield EnterpriseScriptService::ServiceChannel.new(in_writer, out_reader)
      ensure
        code = spawner.wait(pid, Process::WNOHANG) || begin
          begin
            spawner.kill(9, pid)
            spawner.wait(pid)
          rescue Errno::ESRCH
            code = -1
          end
        end

        out_reader.close
        in_writer.close
      end

      code
    end
  end
end
