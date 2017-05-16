#ifndef ENTERPRISE_SCRIPT_SERVICE_ERROR_HPP
#define ENTERPRISE_SCRIPT_SERVICE_ERROR_HPP

#include <exception>
#include "data.hpp"

enum struct status_code {
  ok,
  runtime, // unused
  bad_input,
  initialization_failure,
  bad_syntax,
  code_generation_failure,
  structure_too_deep,
  sandbox_exception, // unused
  overflow,
  unknown_type,
  unknown_ext, // uses unknown_type exception
  bad_symbol,
  memory_corruption,
  memory_usage_error,
  bad_capacity,
  mmap_failed,
  memory_quota_reached,
  instruction_quota_reached,
  io_failure,
  type_error,
  bad_instruction_sequence,
  bad_seccomp_filter,
};

void leave(status_code) __attribute__((noreturn));

class fatal_error : public std::exception {
public:
  fatal_error(status_code err);

  status_code get_err_code() const;

private:
  const status_code err;
};

struct error_base : public std::exception {
  virtual void pack_into(out_packer &) const = 0;
};

class bad_syntax : public error_base {
  std::string filename_;
  std::int32_t line_number_;
  std::int32_t column_;
  std::string message_;

public:
  bad_syntax(
    std::string filename,
    std::int32_t line_number,
    std::int32_t column,
    std::string message);
  void pack_into(out_packer &) const override;
};

class runtime_error : public error_base {
  std::string message_;
  std::vector<std::string> backtrace_;

public:
  runtime_error(
    std::string message,
    std::vector<std::string> backtrace);
  void pack_into(out_packer &) const override;
};

class unknown_type : public error_base {
  msgpack::type::object_type type_;

public:
  unknown_type(msgpack::type::object_type);
  void pack_into(out_packer &) const override;
};

class unknown_ext : public error_base {
  std::int8_t type_;

public:
  unknown_ext(std::int8_t);
  void pack_into(out_packer &) const override;
};

#endif
