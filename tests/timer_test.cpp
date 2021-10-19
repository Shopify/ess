//
// Created by Alex Snaps on 2016-11-23.
//
#include "timer.hpp"
#include "gtest/gtest.h"

TEST(timer_test, measure_cpu_time_scale_on_init) {
  timer t(nullptr);
  EXPECT_GT(t.cpu_time_scale_, std::uint64_t{0});
}

TEST(timer_test, writes_output_once) {
  int invokes = 0;
  auto w = [&invokes](const std::string, const std::int64_t) {
    invokes++;
  };
  timer t(w);
  {
    auto scope = t.measure("foo");
  }
  EXPECT_EQ(invokes, 1);
}

TEST(timer_test, writer_gets_proper_name) {
  auto m_name = "foo";
  auto w = [m_name](const std::string name, const std::int64_t) {
    EXPECT_EQ(name, m_name);
  };
  timer t(w);
  auto scope = t.measure(m_name);
}

TEST(timer_test, writer_gets_proper_timing) {
  std::int64_t time = 42;
  auto w = [time](const std::string, const std::int64_t t) {
    EXPECT_GE(t, time);
  };
  timer t(w);
  {
    auto scope = t.measure("foo");
    usleep(time);
  }
}

TEST(timer_test, get_elapsed_time_us_returns_proper_time) {
  std::int64_t time = 42;
  auto w = [time](const std::string, const std::int64_t) { };
  timer t(w);

  {
    auto scope = t.measure("foo");
    EXPECT_LT(scope.get_elapsed_time_us(), time);
    usleep(time);
    EXPECT_GE(scope.get_elapsed_time_us(), time);
  }
}
