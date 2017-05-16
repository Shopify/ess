Vagrant.configure(2) do |config|
  config.vm.box = "ubuntu/xenial64"
  config.vm.provision(:shell, path: "bootstrap.sh")
  config.ssh.forward_agent = true
end
