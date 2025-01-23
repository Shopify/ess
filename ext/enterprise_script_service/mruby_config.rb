require_relative("./flags")
require_relative("./mruby_shared_config")

build(Flags.io_safe_defines)
crossbuild("sandbox", Flags.defines)
