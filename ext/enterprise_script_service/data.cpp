#include "data.hpp"
#include "error.hpp"
#include "mruby_engine.hpp"
#include "script_runner.hpp"
#include <mruby/array.h>
#include <mruby/hash.h>
#include <mruby/string.h>
#include <unistd.h>


// GENERIC DATA BIT

output_stream &output_stream::write(const char *buffer, std::size_t size) {
  while(size > 0) {
    ssize_t written = ::write(fd, buffer, size);
    if (written == -1) {
      if (errno == EINTR) continue;
      leave(status_code::io_failure);
    }
    buffer += written;
    size -= written;
  }

  return *this;
}

output_stream::output_stream(const int fd) : fd(fd) {}


static void check_depth(int current_depth);

static const auto INVALID_STDOUT_MESSAGE = std::string{"(can't read stdout)"};

data_writer::data_writer(out_packer &packer) : packer(packer) {}

void data_writer::emit_measurement(std::string key, int64_t value) noexcept {
  packer.pack_array(2);
  packer.pack(symbol{"measurement"});
  packer.pack_array(2);
  packer.pack(symbol{key});
  packer.pack_int64(value);
}

// HELPERS

static void emit_ruby_as_msgpack_rec(
    me_mruby_engine &engine,
    mrb_value ruby_value,
    out_packer &packer,
    int depth);


mruby_data_writer::mruby_data_writer(data_writer &writer, me_mruby_engine &engine, std::uint64_t in)
    : writer(writer), engine(engine), in(in) { }

void mruby_data_writer::emit_output() {
  mrb_value output, stdout;
  output = engine.extract("@output");
  stdout = engine.extract("@stdout_buffer");
  writer.packer.pack_array(2);
  writer.packer.pack(symbol{"output"});
  writer.packer.pack_map(2);
  writer.packer.pack(symbol{"extracted"});
  emit_ruby_as_msgpack_rec(engine, output, writer.packer, 0);
  writer.packer.pack(symbol{"stdout"});
  if (mrb_type(stdout) == MRB_TT_STRING) {
    emit_ruby_as_msgpack_rec(engine, stdout, writer.packer, 0);
  } else {
    writer.packer.pack_str((uint32_t) INVALID_STDOUT_MESSAGE.size());
    writer.packer.pack_str_body(INVALID_STDOUT_MESSAGE.data(), (uint32_t) INVALID_STDOUT_MESSAGE.size());
  }
}

void mruby_data_writer::emit_stat() {
  std::uint64_t instructions = engine.instruction_count;
  std::uint64_t total = engine.instruction_total;
  struct meminfo mem_info = me_memory_pool_info(engine.allocator);
  std::uint64_t memory = mem_info.arena - mem_info.fordblks;

  writer.packer.pack_array(2);
  writer.packer.pack(symbol{"stat"});
  writer.packer.pack_map(4);
  writer.packer.pack(symbol{"instructions"});
  writer.packer.pack_int64(instructions);
  writer.packer.pack(symbol{"total_instructions"});
  writer.packer.pack_int64(total);
  writer.packer.pack(symbol{"memory"});
  writer.packer.pack_int64(memory);
  writer.packer.pack(symbol{"bytes_in"});
  writer.packer.pack_uint64(in);
}

mruby_data_writer::~mruby_data_writer() {
  emit_stat();
}

// HELPERS

void emit_ruby_as_msgpack_rec(me_mruby_engine &engine, mrb_value ruby_value, out_packer &packer, int depth) {
  check_depth(depth);

  if (mrb_nil_p(ruby_value)) {
    packer.pack_nil();
    return;
  }

  enum mrb_vtype type = mrb_type(ruby_value);
  switch (type) {
    case MRB_TT_FALSE:
      packer.pack_false();
      return;
    case MRB_TT_TRUE:
      packer.pack_true();
      return;
    case MRB_TT_FIXNUM:
      packer.pack_int64(mrb_fixnum(ruby_value));
      return;
    case MRB_TT_FLOAT:
      packer.pack_double(mrb_float(ruby_value));
      return;
    case MRB_TT_STRING: {
      auto length = RSTRING_LEN(ruby_value);
      if (length > UINT32_MAX) {
        throw fatal_error(status_code::overflow);
      }
      packer.pack_str((uint32_t) length);
      packer.pack_str_body(RSTRING_PTR(ruby_value), (uint32_t) length);
      return;
    }
    case MRB_TT_SYMBOL: {
      auto length = mrb_int{0};
      auto name = mrb_sym2name_len(engine.state, mrb_symbol(ruby_value), &length);
      engine.check_exception();
      if (name == nullptr || length <= 0) {
        throw fatal_error(status_code::bad_symbol);
      }
      packer.pack(symbol{name});
      return;
    }
    case MRB_TT_ARRAY: {
      auto f = RARRAY_LEN(ruby_value);
      if (f > UINT32_MAX) {
        throw fatal_error(status_code::structure_too_deep);
      }
      packer.pack_array((uint32_t) f);
      for (auto i = mrb_int{0}; i < f; ++i) {
        auto element = RARRAY_PTR(ruby_value)[i];
        emit_ruby_as_msgpack_rec(engine, element, packer, depth + 1);
      }
      return;
    }
    case MRB_TT_HASH: {
      struct kh_ht *kh = RHASH_TBL(ruby_value);

      if (kh) {
        auto size = size_t{kh_size(kh)};
        if (size > UINT32_MAX) {
          throw fatal_error(status_code::structure_too_deep);
        }
        packer.pack_map((uint32_t) size);

        for (int i = kh_begin(kh), f = kh_end(kh); i < f; ++i) {
          if (!kh_exist(kh, i)) {
            continue;
          }

          emit_ruby_as_msgpack_rec(engine, kh_key(kh, i), packer, depth + 1);
          emit_ruby_as_msgpack_rec(engine, kh_value(kh, i).v, packer, depth + 1);
        }
      } else {
        packer.pack_map(0);
      }
      return;
    }
    default:
      throw fatal_error(status_code::unknown_type);
  }
}

void check_depth(int current_depth) {
  if (current_depth > 32) {
    throw fatal_error(status_code::structure_too_deep);
  }
}

