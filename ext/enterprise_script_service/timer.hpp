#ifndef ENTERPRISE_SCRIPT_SERVICE_TIMER_HPP
#define ENTERPRISE_SCRIPT_SERVICE_TIMER_HPP

#include <functional>
#include <chrono>
#include <cstdint>
#include <string>

using cpu_time_scale = std::uint64_t;

struct timer {

  std::uint64_t cpu_time_scale_;
  std::function<void(const std::string, const std::int64_t)> writer;

  timer(std::function<void(const std::string, const std::int64_t)> writer);

  struct scope {
    std::string name_;
    std::uint64_t cpu_time_scale_, base_;
    std::function<void(const std::string, const std::int64_t)> writer_;

    std::int64_t get_elapsed_time_us();

    scope(const std::string name, const timer &t);
    scope(scope &&rhs) = default;
    scope(const scope &) = delete;
    scope operator=(const scope &) = delete;
    ~scope();
  };

  scope measure(const std::string name) const;
};

#endif
