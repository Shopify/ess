module EnterpriseScriptService
  class Spawner
    def spawn(*)
      super
    end

    def wait(pid, flags = 0)
      pid, status = Process.wait2(pid, flags)

      status && if status.signaled?
        255 + status.termsig
      else
        status.exitstatus
      end
    end

    def kill(signal, pid)
      Process.kill(signal, pid)
    end
  end
end
