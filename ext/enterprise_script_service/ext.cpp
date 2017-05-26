#include "script_data.hpp"
#include "sandbox.hpp"
#include "timer.hpp"
#include "error.hpp"
#include "script_runner.hpp"
#include "options.hpp"
#include <sys/time.h>
#include <iostream>
#include <unistd.h>

static me_mruby_engine *init_engine(const timer &t, me_memory_pool *allocator, const std::uint64_t instruction_quota);
static me_memory_pool *init_mem_pool(const timer &t, size_t capacity);
static void read_data(script_data &script, const timer &t);
static void sandbox(const timer &t);

int main(int argc, char *argv[]) {
  auto code = status_code::ok;
  try {
    reserve_memory();

    output_stream stream{STDOUT_FILENO};
    out_packer packer{stream};
    data_writer writer(packer);
    auto script = new script_data();

    timer t = timer([&writer](const std::string name, const int64_t timed) {
      writer.emit_measurement(name, timed);
    });

    read_data(*script, t);

    options opts;
    opts.read_from(argc, argv);

    me_memory_pool *allocator = init_mem_pool(t, opts.memory_quota());
    me_mruby_engine *engine = init_engine(t, allocator, opts.instruction_quota());

    sandbox(t);

    script_runner runner(*engine, t);
    runner.run(*script, writer, opts.instruction_quota_start());
  } catch(fatal_error e) {
    code = e.get_err_code();
  }

  leave(code);
}

void sandbox(const timer &t) {
    auto timing = t.measure("sandbox");
    time_t nothing;
    struct tm oblivion;
    gmtime_r(&nothing, &oblivion);
    sandbox();
}

void read_data(script_data &script, const timer &t) {
    auto timing = t.measure("in");
    script.read_from(STDIN_FILENO);
}

me_memory_pool *init_mem_pool(const timer &t, size_t capacity) {
  me_memory_pool *allocator;
  {
    auto timing = t.measure("mem");
    allocator = me_memory_pool_new(capacity);
  }
  return allocator;
}

me_mruby_engine *init_engine(const timer &t, me_memory_pool *allocator, const std::uint64_t instruction_quota) {
  me_mruby_engine *engine;
  {
    auto timing = t.measure("init");
    engine = me_mruby_engine_new(allocator, instruction_quota);
  }
  return engine;
}
