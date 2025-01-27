require_relative("./flags")

MRUBY_ENGINE_GEMBOX_PATH = Pathname.new(__FILE__).dirname.join("mruby_engine")

def build(defines)
  MRuby::Build.new do |conf|
    toolchain(:gcc)

    enable_debug

    conf.gembox(MRUBY_ENGINE_GEMBOX_PATH)
    conf.gem(core: "mruby-bin-mirb")
    conf.gem(core: 'mruby-bin-mruby')

    conf.bins = ["mrbc", "mruby"]

    conf.cc do |cc|
      cc.flags += %w(-fPIC)
      cc.flags += Flags.cflags
      cc.defines += defines
    end
  end
end

def crossbuild(directory, defines)
  MRuby::CrossBuild.new(directory) do |conf|
    toolchain(:gcc)

    enable_debug

    conf.gembox(MRUBY_ENGINE_GEMBOX_PATH)

    conf.bins = []

    conf.cc do |cc|
      cc.flags += %w(-fPIC)
      cc.flags += Flags.cflags
      cc.defines += defines
    end
  end
end
