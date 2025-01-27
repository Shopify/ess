require_relative("./flags")
require_relative("./mruby_shared_config")

build(Flags.io_safe_defines_with_no_gc)
crossbuild("sandbox_no_gen_gc", Flags.defines_with_no_gc)
