Gem::Specification.new do |spec|
  spec.name = "enterprise_script_service"
  spec.version = "0.1.0"
  spec.date = "2016-10-18"
  spec.summary = "A service that evaluates ruby scripts in a secure way."
  spec.authors = ["Simon Génier"]
  spec.email = "simon.genier@shopify.com"
  spec.files = %w(
  )
  spec.extensions = ["ext/enterprise_script_service/Rakefile"]
  spec.homepage = "https://github.com/Shopify/enterprise-script-service"
  spec.license = "Proprietary"
  spec.required_ruby_version = '>= 2.2'

  spec.add_dependency("msgpack", "~> 1.0")
  spec.add_dependency("getoptlong") # mruby uses this
  spec.add_development_dependency("bundler")
  # Newer versions don't work with Ruby 2.5 and 2.6
  spec.add_development_dependency("rake")
  spec.add_development_dependency("rake-compiler")
  spec.add_development_dependency("rspec", "~> 3.5")
end
