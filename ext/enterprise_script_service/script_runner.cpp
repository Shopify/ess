//
// Created by Alex Snaps on 2016-11-29.
//

#include "script_runner.hpp"
#include "error.hpp"
#include <mruby/proc.h>

script_runner::script_runner(me_mruby_engine &engine, timer &timer)
    : engine_(engine), timer_(timer) { }

bool script_runner::run(script_data &script, data_writer &writer, unsigned int instruction_quota_start) {
  auto success = true;
  mruby_data_writer engine_writer(writer, engine_, script.size());
  try {
    engine_.limit_instructions = !instruction_quota_start;
    mrb_value value;
    {
      auto timing = timer_.measure("decode");
      value = script.input(engine_);
    }
    {
      auto t2 = timer_.measure("inject");
      engine_.inject("@input", value);
    }

    {
      auto timing = timer_.measure("lib");
      auto &data = script.library();
      if (data.size() > 0) {
        engine_.load_instruction_sequence(data);
      }
    }

    unsigned int index = 0;
    for (auto &&source : script.sources()) {
      if (++index > instruction_quota_start && !engine_.limit_instructions) {
        engine_.limit_instructions = true;
      }
      try {
        RProc *pProc;
        {
          auto timing = timer_.measure("compile");
          pProc = engine_.generate_code(source);
        }

        {
          auto timing = timer_.measure("eval");
          engine_.eval(pProc);
        }
      } catch (error_base &err) {
        success = false;
        err.pack_into(writer.packer);
      }
    }

    {
      auto timing = timer_.measure("out");
      engine_writer.emit_output();
    }
  } catch (error_base &err) {
    engine_.limit_instructions = true;
    err.pack_into(writer.packer);
    return false;
  }

  engine_.limit_instructions = true;
  return success;
}
