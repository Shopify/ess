#include "timer.hpp"
#include <cinttypes>

static std::uint64_t read_time_stamp_counter() {
  std::uint32_t low, high;
  asm ("rdtsc" : "=a" (low), "=d" (high) :: "memory");
  return (std::uint64_t{high} << 32) | low;
}

static std::uint64_t measure_cpu_time_scale() {
  auto start_tick = read_time_stamp_counter();
  auto start_time = std::chrono::high_resolution_clock::now();

  std::uint64_t end_tick;
  std::chrono::high_resolution_clock::time_point end_time;
  std::chrono::nanoseconds diff;
  do {
    end_tick = read_time_stamp_counter();
    end_time = std::chrono::high_resolution_clock::now();
    diff = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
  } while (diff < std::chrono::microseconds{200});

  return (end_tick - start_tick) / std::chrono::duration_cast<std::chrono::microseconds>(diff).count();
}

timer::timer(std::function<void(const std::string, const std::int64_t)> writer) :
    cpu_time_scale_(measure_cpu_time_scale()), writer(writer)
{ }

timer::scope timer::measure(const std::string name) const {
  auto s = timer::scope(name, *this);
  return s;
}

std::int64_t timer::scope::get_elapsed_time_us() {
  auto now = read_time_stamp_counter();
  return std::chrono::microseconds((now - base_) / cpu_time_scale_).count();
}

timer::scope::scope(const std::string name, const timer& t)
  : name_(name)
  , cpu_time_scale_(t.cpu_time_scale_)
  , base_(read_time_stamp_counter())
  , writer_(t.writer)
{ }

timer::scope::~scope() {
  if (!name_.empty() && writer_) {
    writer_(name_, this->get_elapsed_time_us());
  }
}
