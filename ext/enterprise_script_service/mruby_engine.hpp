#ifndef ENTERPRISE_SCRIPT_SERVICE_MRUBY_ENGINE_H
#define ENTERPRISE_SCRIPT_SERVICE_MRUBY_ENGINE_H

#include "units.hpp"
#include "memory_pool.hpp"
#include <mruby.h>
#include <cstdint>
#include <string>
#include <vector>

struct ruby_source {
  ruby_source(std::string path_, std::string source_)
      : path(path_)
      , source(source_) { }

  std::string path;
  std::string source;
};

struct me_mruby_engine {
  void inject(const std::string &ivar_name, mrb_value &value);
  mrb_value extract(const std::string &ivar_name);
  struct RProc *generate_code(const ruby_source &ruby_src);
  void load_instruction_sequence(const std::vector<std::uint8_t> &data);
  void eval(struct RProc *proc);
  void check_exception();

  struct mrb_state *state;
  struct me_memory_pool *allocator;

  std::uint64_t instruction_total;
  std::uint64_t instruction_count;
  std::uint64_t instruction_quota;
  std::uint64_t execution_time_us;
  bool limit_instructions;
  bool quota_error_raised;
  std::int64_t ctx_switches_v;
  std::int64_t ctx_switches_iv;
  std::int64_t cpu_time_ns;
};

struct me_mruby_engine *me_mruby_engine_new(
  struct me_memory_pool *allocator,
  uint64_t instruction_limit);
void me_mruby_engine_destroy(struct me_mruby_engine *self);

struct me_memory_pool *me_mruby_engine_get_allocator(struct me_mruby_engine *self);
uint64_t me_mruby_engine_get_instruction_count(struct me_mruby_engine *self);
struct meminfo me_mruby_engine_get_memory_info(struct me_mruby_engine *self);
int64_t me_mruby_engine_get_ctx_switches_voluntary(struct me_mruby_engine *self);
int64_t me_mruby_engine_get_ctx_switches_involuntary(struct me_mruby_engine *self);
int64_t me_mruby_engine_get_cpu_time(struct me_mruby_engine *self);
bool me_mruby_engine_get_quota_exception_raised(struct me_mruby_engine *self);

#endif
