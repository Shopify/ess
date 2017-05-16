#include "data.hpp"
#include "error.hpp"
#include <unistd.h>

#ifdef __linux__

#include <sys/syscall.h>

void leave(status_code sc) {
  for (;;) {
    syscall(SYS_exit, static_cast<long>(sc));
  }
}

#else

#include <cstdlib>

void leave(status_code sc) {
  std::exit(static_cast<int>(sc));
}

#endif

bad_syntax::bad_syntax(
  std::string filename,
  std::int32_t line_number,
  std::int32_t column,
  std::string message)
  : filename_(filename)
  , line_number_(line_number)
  , column_(column)
  , message_(message) { }

void bad_syntax::pack_into(out_packer &packer) const {
  packer.pack_array(2);
  packer.pack(symbol{"error"});
  packer.pack_map(5);
  packer.pack(symbol{"__type"});
  packer.pack(symbol{"syntax"});
  packer.pack(symbol{"filename"});
  packer.pack(filename_);
  packer.pack(symbol{"line_number"});
  packer.pack(line_number_);
  packer.pack(symbol{"column"});
  packer.pack(column_);
  packer.pack(symbol{"message"});
  packer.pack(message_);
}

runtime_error::runtime_error(
  std::string message,
  std::vector<std::string> backtrace)
  : message_(message)
  , backtrace_(backtrace) { }

void runtime_error::pack_into(out_packer &packer) const {
  packer.pack_array(2);
  packer.pack(symbol{"error"});
  packer.pack_map(3);
  packer.pack(symbol{"__type"});
  packer.pack(symbol{"runtime"});
  packer.pack(symbol{"message"});
  packer.pack(message_);
  packer.pack(symbol{"backtrace"});
  packer.pack(backtrace_);
}

unknown_type::unknown_type(msgpack::type::object_type type)
  : type_(type) { }

void unknown_type::pack_into(out_packer &packer) const {
  packer.pack_array(2);
  packer.pack(symbol{"error"});
  packer.pack_map(2);
  packer.pack(symbol{"__type"});
  packer.pack(symbol{"unknown_type"});
  packer.pack(symbol{"type"});
  packer.pack(std::uint32_t{type_});
}

unknown_ext::unknown_ext(std::int8_t type)
  : type_(type) { }

void unknown_ext::pack_into(out_packer &packer) const {
  packer.pack_array(2);
  packer.pack(symbol{"error"});
  packer.pack_map(2);
  packer.pack(symbol{"__type"});
  packer.pack(symbol{"unknown_ext"});
  packer.pack(symbol{"type"});
  packer.pack(type_);
}

fatal_error::fatal_error(status_code err) : err(err) {}

status_code fatal_error::get_err_code() const {
  return err;
}
