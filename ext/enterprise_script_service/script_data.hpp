//
// Created by Alex Snaps on 2016-11-29.
//

#ifndef ENTERPRISE_SCRIPT_SERVICE_SCRIPT_DATA_HPP
#define ENTERPRISE_SCRIPT_SERVICE_SCRIPT_DATA_HPP

#include <msgpack.hpp>
#include "mruby_engine.hpp"

class script_data {
public:
  void read_from(int fd);
  const std::vector<ruby_source> &sources() const;
  const std::vector<uint8_t> &library() const;
  const mrb_value input(me_mruby_engine &engine) const;
  void sources(const std::vector<ruby_source> &sources);
  std::uint64_t size();

private:
  msgpack::object input_;
  std::vector<ruby_source> sources_;
  std::vector<uint8_t> library_;
  msgpack::object_handle result;
  std::uint64_t in_;
};


#endif //ENTERPRISE_SCRIPT_SERVICE_SCRIPT_DATA_HPP
