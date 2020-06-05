require "bundler/gem_tasks"
require "rake/extensiontask"

begin
  require "rspec/core/rake_task"
  RSpec::Core::RakeTask.new(:spec)
rescue LoadError
end

task(compile: []) do
  Dir.chdir("ext/enterprise_script_service") do
    if RUBY_VERSION >= '2.7'
      sed = 'sed'
      sed = 'gsed' if RUBY_PLATFORM.match?(/darwin/i)
      sh(sed, "-i", "-e", 's/{ :verbose => $verbose }/verbose: $verbose/', "mruby/Rakefile")
    end
    sh("../../bin/rake")
  end
end

Rake::Task[:clean].enhance do
  sh("script/mkmruby", "clean")
end

Rake::Task[:clobber].enhance do
  sh("script/mkmruby", "clobber")
end

task(mrproper: []) do
  Dir.chdir("ext/enterprise_script_service") do
    sh("../../bin/rake", "mrproper")
  end
end

task(spec: [:compile])

task(default: [:spec])
