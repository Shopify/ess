require_relative("./flags")

mruby_engine_gembox_path = Pathname.new(__FILE__).dirname.join("mruby_engine")

MRuby::Build.new do |conf|
  toolchain(:gcc)

  enable_debug

  conf.gembox(mruby_engine_gembox_path)
  conf.gem(core: "mruby-bin-mirb")
  conf.gem(core: 'mruby-bin-mruby')

  conf.bins = ["mrbc", "mruby"]

  conf.cc do |cc|
    cc.flags += %w(-fPIC)
    cc.flags += Flags.cflags
    cc.defines += Flags.io_safe_defines
  end
end

MRuby::CrossBuild.new("sandbox") do |conf|
  toolchain(:gcc)

  enable_debug

  conf.gembox(mruby_engine_gembox_path)

  conf.bins = []

  conf.cc do |cc|
    cc.flags += %w(-fPIC)
    cc.flags += Flags.cflags
    cc.defines += Flags.defines
  end
end
