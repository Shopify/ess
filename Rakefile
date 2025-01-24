require "bundler/gem_tasks"
require "rake/extensiontask"

begin
  require "rspec/core/rake_task"
  RSpec::Core::RakeTask.new(:spec)
rescue LoadError
end

task(compile: []) do
  Dir.chdir("ext/enterprise_script_service") do
    extra_args = []
    extra_args << '' if RUBY_PLATFORM.match?(/darwin/i)
    sh('sed', "-i", *extra_args, 's/{ :verbose => $verbose }/verbose: $verbose/', "mruby/Rakefile")
    sh("../../bin/rake")
  end
end

task(clean: []) do
  Dir.chdir("ext/enterprise_script_service") do
    sh("../../bin/rake", "clean")
  end
end

task(mrproper: []) do
  Dir.chdir("ext/enterprise_script_service") do
    sh("../../bin/rake", "mrproper")
  end
end

task(spec: [:compile])

task(default: [:spec])
