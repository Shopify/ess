#include "mruby_engine.hpp"
#include "error.hpp"
#include <mruby.h>
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/dump.h>
#include <mruby/error.h>
#include <mruby/hash.h>
#include <mruby/opcode.h>
#include <mruby/proc.h>
#include <mruby/string.h>
#include <mruby/throw.h>
#include <mruby/variable.h>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <vector>

void me_mruby_engine::inject(const std::string &ivar_name, mrb_value &value) {
  auto mruby_ivar_name = mrb_intern(state, ivar_name.data(), ivar_name.size());
  mrb_iv_set(this->state, mrb_top_self(this->state), mruby_ivar_name, value);
}

mrb_value me_mruby_engine::extract(const std::string &ivar_name) {
  auto mruby_ivar_name = mrb_intern(state, ivar_name.data(), ivar_name.size());
  return mrb_iv_get(this->state, mrb_top_self(this->state), mruby_ivar_name);
}

void me_mruby_engine::eval(struct RProc *proc) {
  mrb_context_run(this->state, proc, mrb_top_self(this->state), 0);
  this->check_exception();
}

struct RProc *me_mruby_engine::generate_code(const ruby_source &ruby_src) {
  auto context = mrbc_context_new(this->state);
  context->no_exec = true;
  context->capture_errors = true;
  mrbc_filename(this->state, context, ruby_src.path.c_str());

  auto parser_state = mrb_parser_new(this->state);
  auto &source = ruby_src.source;
  parser_state->s = source.data();
  parser_state->send = source.data() + source.size();

  struct mrb_jmpbuf *previous = state->jmp;
  struct mrb_jmpbuf p_jmp;
  state->jmp = &p_jmp;
  MRB_TRY(state->jmp) {
    mrb_parser_parse(parser_state, context);
  }
  MRB_CATCH(state->jmp) {
    state->exc = NULL;
    if (parser_state->nerr < 1) {
      mrb_parser_free(parser_state);
      state->jmp = previous;
      leave(status_code::bad_syntax);
    }
  }
  MRB_END_EXC(state->jmp);
  state->jmp = previous;

  if (parser_state->nerr > 0) {
    mrb_parser_free(parser_state);
    throw bad_syntax(
      parser_state->filename,
      parser_state->error_buffer[0].lineno,
      parser_state->error_buffer[0].column,
      parser_state->error_buffer[0].message);
  }

  auto proc = mrb_generate_code(state, parser_state);
  if (proc == NULL) {
    leave(status_code::code_generation_failure);
  }

  mrb_parser_free(parser_state);
  mrbc_context_free(this->state, context);
  return proc;
}

void me_mruby_engine::load_instruction_sequence(
  const std::vector<std::uint8_t> &data)
{
  auto irep = mrb_read_irep(this->state, data.data());
  if (irep == nullptr) {
    leave(status_code::bad_instruction_sequence);
  }

  auto proc = mrb_proc_new(this->state, irep);
  this->eval(proc);
}

void me_mruby_engine::check_exception() {
  if (state->exc == nullptr) {
    return;
  }

  auto exception = mrb_obj_value(state->exc);
  state->exc = nullptr;
  auto exit_exception_class = mrb_class_get(state, "ExitException");
  if (mrb_obj_is_kind_of(state, exception, exit_exception_class)) {
    return;
  }

  auto ruby_backtrace = mrb_exc_backtrace(state, exception);
  check_exception();
  if (mrb_type(ruby_backtrace) != MRB_TT_ARRAY) {
    leave(status_code::type_error);
  }

  auto i = RARRAY_PTR(ruby_backtrace);
  auto f = RARRAY_PTR(ruby_backtrace) + RARRAY_LEN(ruby_backtrace);
  std::vector<std::string> backtrace{};
  std::for_each(i, f, [&backtrace](mrb_value location) {
    if (mrb_type(location) != MRB_TT_STRING) {
      leave(status_code::type_error);
    }

    backtrace.emplace_back(std::string{
      RSTRING_PTR(location),
      static_cast<std::size_t>(RSTRING_LEN(location))});
  });

  auto ruby_message = mrb_obj_as_string(state, exception);
  check_exception();
  if (mrb_type(ruby_message) != MRB_TT_STRING) {
    leave(status_code::type_error);
  }
  std::string message{
    RSTRING_PTR(ruby_message),
    static_cast<std::size_t>(RSTRING_LEN(ruby_message))};

  throw runtime_error{message, backtrace};
}

static void *mruby_engine_allocf(struct mrb_state *state, void *block, size_t size, void *data) {
  (void)state;

  auto engine = reinterpret_cast<me_mruby_engine *>(data);

  if (size == 0) {
    if (block != NULL) {
      me_memory_pool_free(engine->allocator, block);
    }
    return NULL;
  }

  if (block == NULL) {
    block = me_memory_pool_malloc(engine->allocator, size);
  } else {
    block = me_memory_pool_realloc(engine->allocator, block, size);
  }

  if (block == NULL) {
    leave(status_code::memory_quota_reached);
  }
  return block;
}

bool me_mruby_engine_get_quota_exception_raised(struct me_mruby_engine *self) {
  return self->quota_error_raised;
}

static void mruby_engine_code_fetch_hook(
  struct mrb_state* mrb,
  struct mrb_irep *irep,
  mrb_code *pc,
  mrb_value *regs)
{
  (void)irep;
  (void)pc;
  (void)regs;

  auto engine = reinterpret_cast<me_mruby_engine *>(mrb->allocf_ud);

  if (engine->instruction_count >= engine->instruction_quota) {
    leave(status_code::instruction_quota_reached);
  }

  engine->instruction_total++;
  if (engine->limit_instructions) {
    engine->instruction_count++;
  }
}

static mrb_value mruby_engine_exit(struct mrb_state *state, mrb_value rvalue) {
  mrb_raise(state, mrb_class_get(state, "ExitException"), "exit exception");
  return rvalue;
}

struct me_mruby_engine *me_mruby_engine_new(
  struct me_memory_pool *allocator,
  uint64_t instruction_quota)
{
  auto self = reinterpret_cast<me_mruby_engine *>(
    me_memory_pool_malloc(allocator, sizeof(struct me_mruby_engine)));
  self->allocator = allocator;
  self->state = mrb_open_allocf(mruby_engine_allocf, self);

  if (self->state == nullptr) {
    leave(status_code::initialization_failure);
  }

  mrb_define_class(self->state, "ExitException", mrb_class_get(self->state, "Exception"));
  mrb_define_method(self->state , self->state->kernel_module, "exit", mruby_engine_exit, 1);

  self->instruction_quota = instruction_quota;
  self->instruction_count = 0;
  self->instruction_total = 0;
  self->limit_instructions = true;
  self->quota_error_raised = false;
  self->state->code_fetch_hook = mruby_engine_code_fetch_hook;
  self->ctx_switches_v = -1;
  self->ctx_switches_iv = -1;
  self->cpu_time_ns = 0;

  return self;
}

void me_mruby_engine_destroy(struct me_mruby_engine *self) {
  struct me_memory_pool *allocator = me_mruby_engine_get_allocator(self);
  mrb_close(self->state);
  me_memory_pool_free(allocator, self);
}

struct me_memory_pool *me_mruby_engine_get_allocator(struct me_mruby_engine *self) {
  return self->allocator;
}

uint64_t me_mruby_engine_get_instruction_count(struct me_mruby_engine *self) {
  return self->instruction_count;
}

struct meminfo me_mruby_engine_get_memory_info(struct me_mruby_engine *self) {
  return me_memory_pool_info(self->allocator);
}

int64_t me_mruby_engine_get_ctx_switches_voluntary(struct me_mruby_engine *self) {
  return self->ctx_switches_v;
}

int64_t me_mruby_engine_get_ctx_switches_involuntary(struct me_mruby_engine *self) {
  return self->ctx_switches_iv;
}

int64_t me_mruby_engine_get_cpu_time(struct me_mruby_engine *self) {
  return self->cpu_time_ns;
}
