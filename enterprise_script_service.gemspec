require "pathname"

lib = File.expand_path("../lib", __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)

require "enterprise_script_service/version"

Gem::Specification.new do |spec|
  spec.name = "enterprise_script_service"
  spec.version = EnterpriseScriptService::VERSION
  spec.date = "2016-10-18"
  spec.summary = "A service that evaluates ruby scripts in a secure way."
  spec.authors = ["Simon Génier"]
  spec.email = "simon.genier@shopify.com"
  spec.files = begin
  submodules =
    %x(git submodule status --recursive).split("\n").map do |submodule|
      submodule.split(/\(|\s+/)[2]
    end.compact

  list_tracked_files = lambda do |dir|
    Dir.chdir(Pathname.new(__FILE__).dirname.join(dir)) do
      %x(git ls-files -z).split("\x0").map do |file|
        Pathname.new(dir).join(file).to_s
      end
    end
  end

  list_tracked_files.call(".") + submodules.flat_map do |submodule|
    list_tracked_files.call(submodule)
  end
end

  spec.extensions = ["ext/enterprise_script_service/Rakefile"]
  spec.homepage = "https://github.com/Shopify/enterprise-script-service"
  spec.license = "MIT"
  spec.required_ruby_version = '>= 2.5'

  spec.add_dependency("msgpack", "~> 1.0")
  spec.add_development_dependency("bundler")
  spec.add_development_dependency("pry-byebug", "~> 3.4")
  spec.add_development_dependency("rake", "~> 13.2")
  spec.add_development_dependency("rake-compiler", "~> 0.9")
  spec.add_development_dependency("rspec", "~> 3.5")
end
