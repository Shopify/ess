//
// Created by Alex Snaps on 2016-11-29.
//

#ifndef ENTERPRISE_SCRIPT_SERVICE_SCRIPT_RUNNER_HPP
#define ENTERPRISE_SCRIPT_SERVICE_SCRIPT_RUNNER_HPP


#include "mruby_engine.hpp"
#include "script_data.hpp"
#include "timer.hpp"
#include "data.hpp"

class script_runner {
public:
  script_runner(me_mruby_engine &engine, timer &timer);
  bool run(script_data &script, data_writer &writer, unsigned int instruction_quota_start = 0);

private:
  me_mruby_engine &engine_;
  timer &timer_;
};

class mruby_data_writer {
public:
  mruby_data_writer(data_writer &writer, me_mruby_engine &engine, std::uint64_t in = 0);
  virtual ~mruby_data_writer();
  void emit_output();
  void emit_stat();

private:
  data_writer &writer;
  me_mruby_engine &engine;
  std::uint64_t in;
};


#endif //ENTERPRISE_SCRIPT_SERVICE_SCRIPT_RUNNER_HPP
