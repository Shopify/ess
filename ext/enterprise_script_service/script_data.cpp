//
// Created by Alex Snaps on 2016-11-29.
//

#include "script_data.hpp"

#include <unistd.h>
#include <mruby/array.h>
#include <mruby/hash.h>
#include "error.hpp"
#include "data.hpp"

static const std::size_t FIRST_CHUNK_SIZE = 4; // big enough to read a uint64_t from msgpack, given our sizes
static const std::size_t MSGPACK_CHUNK_SIZE = 256 * KiB; // ~ msgpack size to then blow the 4MB mem quota

static bool equal_to_symbol_p(const msgpack::object &object, const std::string &string);
static msgpack::object find_in(const msgpack::object &handle, const char string[6]);

static std::vector<ruby_source> unpack_sources(const msgpack::object &object);
static std::vector<std::uint8_t> fetch_library(const msgpack::object &object);

static mrb_value msgpack_to_ruby(me_mruby_engine &engine, const msgpack::object &msgpack_value, int depth = 0);


void script_data::read_from(int fd) {
  msgpack::unpacker unpacker;
  msgpack::object sources;
  msgpack::object library;
  size_t expected_size = FIRST_CHUNK_SIZE;

  in_ = 0;
  for (;;) {
    unpacker.reserve_buffer(expected_size);
    auto input_size = read(fd, unpacker.buffer(), expected_size);
    if (input_size < 0) {
      throw fatal_error(status_code::io_failure);
    }
    in_ += input_size;
    if (expected_size <= FIRST_CHUNK_SIZE && in_ > 0) {
      // done with first chunk, fallback to larger chunks in case we don't get a size hint
      expected_size = MSGPACK_CHUNK_SIZE;
    }
    unpacker.buffer_consumed(static_cast<std::size_t>(input_size));

    if (unpacker.next(result)) {
      if (result.get().type == msgpack::v2::type::POSITIVE_INTEGER) {
        // we got a payload size hint! Use that to read the whole payload in one big chunk...
        // and deal with the EXT issue when read in multiple chunks
        expected_size = result.get().via.u64;
        if(!unpacker.next(result)) {
          continue;
        }
      }
      this->input_ = find_in(result.get(), "input");
      sources = find_in(result.get(), "sources");
      if (this->input_.is_nil() || sources.is_nil()) {
        throw fatal_error(status_code::bad_input);
      }
      library = find_in(result.get(), "library");
      break;
    }
  }
  this->sources_ = unpack_sources(sources);
  if (!library.is_nil()) {
    this->library_ = fetch_library(library);
  }
}

const std::vector<ruby_source> &script_data::sources() const {
  return sources_;
}

const std::vector<uint8_t> &script_data::library() const {
  return library_;
}

const mrb_value script_data::input(me_mruby_engine &engine) const {
  return msgpack_to_ruby(engine, input_);
}

void script_data::sources(const std::vector<ruby_source> &sources) {
  sources_ = sources;
}

std::uint64_t script_data::size() {
  return in_;
}

// HELPERS

msgpack::object find_in(const msgpack::object &object, const char *key) {
  msgpack::object nil;
  if (object.type == msgpack::type::MAP) {
    for (auto &element : object.via.map) {
      if (equal_to_symbol_p(element.key, key)) {
        return element.val;
      }
    }
  }

  return nil;
}


bool equal_to_symbol_p(const msgpack::object &object, const std::string &string) {
  if (object.type != msgpack::type::EXT) {
    return false;
  }

  msgpack::object_ext ext = object.via.ext;
  if (ext.type() != SYMBOL_EXT_CODE) {
    return false;
  }

  return ext.size == string.length() && std::equal(ext.data(), ext.data() + ext.size, string.begin());
}

std::vector<ruby_source> unpack_sources(const msgpack::object &object) {
  if (object.type != msgpack::type::ARRAY) {
    throw fatal_error(status_code::bad_input);
  }

  auto sources = std::vector<ruby_source>{};
  for (auto &element : object.via.array) {
    if (element.type != msgpack::type::ARRAY || element.via.array.size != 2) {
      throw fatal_error(status_code::bad_input);
    }

    auto path = element.via.array.ptr[0], source = element.via.array.ptr[1];
    if (path.type != msgpack::type::STR || source.type != msgpack::type::STR) {
      throw fatal_error(status_code::bad_input);
    }

    sources.emplace_back(ruby_source{path.as<std::string>(), source.as<std::string>()});
  }
  return sources;
}

std::vector<std::uint8_t> fetch_library(const msgpack::object &object) {
  if (object.type != msgpack::type::BIN) {
    throw fatal_error(status_code::bad_input);
  }
  return object.as<std::vector<std::uint8_t>>();
}

void check_depth(int current_depth) {
  if (current_depth > 32) {
    throw fatal_error(status_code::structure_too_deep);
  }
}

static bool integer_p(const msgpack::object &msgpack_value) {
  auto type = msgpack_value.type;
  return
      type == msgpack::type::POSITIVE_INTEGER ||
      type == msgpack::type::NEGATIVE_INTEGER;
}

static bool float_p(const msgpack::object &msgpack_value) {
  auto type = msgpack_value.type;
  return type == msgpack::type::FLOAT;
}

mrb_value msgpack_to_ruby(me_mruby_engine &engine, const msgpack::object &msgpack_value, int depth) {
  check_depth(depth);

  if (msgpack_value.type == msgpack::type::NIL) {
    return mrb_nil_value();
  } else if (msgpack_value.type == msgpack::type::BOOLEAN) {
    if (msgpack_value.as<bool>()) {
      auto value = mrb_true_value();
      engine.check_exception();
      return value;
    } else {
      auto value = mrb_false_value();
      engine.check_exception();
      return value;
    }
  } else if (integer_p(msgpack_value)) {
    return mrb_fixnum_value(mrb_int{msgpack_value.as<long>()});
  } else if (float_p(msgpack_value)) {
    auto ruby_value = mrb_float_value(engine.state, mrb_float{msgpack_value.as<double>()});
    engine.check_exception();
    return ruby_value;
  } else if (msgpack_value.type == msgpack::type::STR) {
    auto string_value = msgpack_value.as<std::string>();
    auto ruby_value = mrb_str_new(engine.state, string_value.data(), string_value.size());
    engine.check_exception();
    return ruby_value;
  } else if (msgpack_value.type == msgpack::type::BIN) {
    auto string_value = msgpack_value.as<std::vector<char>>();
    auto ruby_value = mrb_str_new(engine.state, string_value.data(), string_value.size());
    engine.check_exception();
    return ruby_value;
  } else if (msgpack_value.type == msgpack::type::ARRAY) {
    auto array = mrb_ary_new(engine.state);
    engine.check_exception();
    for (auto &element : msgpack_value.via.array) {
      mrb_ary_push(engine.state, array, msgpack_to_ruby(engine, element, depth + 1));
      engine.check_exception();
    }
    return array;
  } else if (msgpack_value.type == msgpack::type::MAP) {
    auto hash = mrb_hash_new(engine.state);
    engine.check_exception();
    for (auto &element : msgpack_value.via.map) {
      mrb_hash_set(
          engine.state,
          hash,
          msgpack_to_ruby(engine, element.key, depth + 1),
          msgpack_to_ruby(engine, element.val, depth + 1));
      engine.check_exception();
    }
    return hash;
  } else if (msgpack_value.type == msgpack::type::EXT) {
    auto ext = msgpack_value.via.ext;
    switch (ext.type()) {
      case SYMBOL_EXT_CODE: {
        auto symbol = mrb_intern(engine.state, ext.data(), ext.size);
        engine.check_exception();
        return mrb_symbol_value(symbol);
      }
      default:
        throw unknown_ext{ext.type()};
    }
  } else {
    throw unknown_type{msgpack_value.type};
  }
}

