#include "timer.hpp"
#include <cinttypes>

timer::timer(std::function<void(const std::string, const std::int64_t)> writer) :
  writer(writer)
{ }

timer::scope timer::measure(const std::string name) const {
  auto s = timer::scope(name, *this);
  return s;
}

std::int64_t timer::scope::get_elapsed_time_us() {
  auto now = std::chrono::steady_clock::now();
  return std::chrono::duration_cast<std::chrono::microseconds>(now - base_).count();
}

timer::scope::scope(const std::string name, const timer& t)
  : name_(name)
  , base_(std::chrono::steady_clock::now())
  , writer_(t.writer)
{ }

timer::scope::~scope() {
  if (!name_.empty() && writer_) {
    writer_(name_, this->get_elapsed_time_us());
  }
}
