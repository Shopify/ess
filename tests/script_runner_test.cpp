//
// Created by Alex Snaps on 2016-12-06.
//

#include "script_runner.hpp"
#include "gtest/gtest.h"

static const int BUFSIZE = 1024;

TEST(script_runner_test, defaults_to_counting_all_instructions) {
  int fd[2];
  if (pipe(fd) == -1) {
    perror("pipe");
    FAIL();
  }

  output_stream stream{fd[1]};
  out_packer packer{stream};
  data_writer writer(packer);

  me_memory_pool *allocator = me_memory_pool_new(4 * MiB);
  me_mruby_engine *engine = me_mruby_engine_new(allocator, 100000);

  std::vector<ruby_source> sources;
  sources.push_back({"A", "@output = 'nevemind'"});
  sources.push_back({"B", "@output = 'yay!'"});
  timer t([](const std::string, const int64_t) {});
  script_runner runner(*engine, t);
  script_data script;
  script.sources(sources);
  runner.run(script, writer);
  close(fd[1]);
  auto instruction_total = engine->instruction_total;
  auto instruction_count = engine->instruction_count;
  auto execution_time_us = engine->execution_time_us;
  me_mruby_engine_destroy(engine);
  me_memory_pool_destroy(allocator);
  close(fd[0]);
  EXPECT_EQ(instruction_total, instruction_count);
  EXPECT_EQ(execution_time_us, std::int64_t{0});
}

TEST(script_runner_test, can_bypass_deserialization_instructions) {
  int fd[2];
  if (pipe(fd) == -1) {
    perror("pipe");
    FAIL();
  }

  output_stream stream{fd[1]};
  out_packer packer{stream};
  data_writer writer(packer);

  me_memory_pool *allocator = me_memory_pool_new(4 * MiB);
  me_mruby_engine *engine = me_mruby_engine_new(allocator, 100000);

  std::vector<ruby_source> sources;
  sources.push_back({"A", "@output = 'nevemind'"});
  sources.push_back({"B", "@output = 'yay!'"});
  timer t([](const std::string, const int64_t) {});
  script_runner runner(*engine, t);
  script_data script;
  script.sources(sources);
  runner.run(script, writer, 1);
  close(fd[1]);
  auto instruction_total = engine->instruction_total;
  auto instruction_count = engine->instruction_count;
  auto execution_time_us = engine->execution_time_us;
  me_mruby_engine_destroy(engine);
  me_memory_pool_destroy(allocator);
  close(fd[0]);
  EXPECT_LT(instruction_count, instruction_total);
  EXPECT_EQ(instruction_count, std::uint64_t{3});
  EXPECT_EQ(execution_time_us, std::int64_t{0});
}

TEST(script_runner_test, runs_all_scripts) {
  int fd[2];
  if(pipe(fd) == -1) {
    perror("pipe");
    FAIL();
  }

  output_stream stream{fd[1]};
  out_packer packer{stream};
  data_writer writer(packer);

  me_memory_pool *allocator = me_memory_pool_new(4 * MiB);
  me_mruby_engine *engine = me_mruby_engine_new(allocator, 100000);

  std::vector<ruby_source> sources;
  sources.push_back({"A", "exit"});
  sources.push_back({"B", "a = 'yay' ; 3.times { a += '!' } ; @output = a"});
  timer t([](const std::string, const int64_t) {});
  script_runner runner(*engine, t);
  script_data script;
  script.sources(sources);
  runner.run(script, writer);
  close(fd[1]);
  EXPECT_EQ(engine->instruction_total, engine->instruction_count);
  EXPECT_GT(engine->execution_time_us, std::int64_t{0});
  me_mruby_engine_destroy(engine);
  me_memory_pool_destroy(allocator);

  char output[BUFSIZE];
  ssize_t r, in = 0;
  while ((r = read(fd[0], output + in, (size_t) (BUFSIZE - in))) > 0) {
    if ((in += r) >= BUFSIZE) break;
  }

  msgpack::object_handle oh = msgpack::unpack(output, in);
  close(fd[0]);

  auto object = oh.get();
  EXPECT_EQ(msgpack::type::ARRAY, object.type);
  EXPECT_EQ(uint32_t{2}, object.via.array.size);
  int i = 0;
  for (auto &&item : object.via.array) {
    switch (i++) {
      case 0 :
        EXPECT_EQ(msgpack::type::EXT, item.type);
        EXPECT_EQ(strncmp("output", item.via.ext.data(), 6), 0);
        break;
      case 1:
        EXPECT_EQ(msgpack::type::MAP, item.type);
        EXPECT_EQ(uint32_t{2}, item.via.map.size);
        for (auto && element: item.via.map) {
          EXPECT_EQ(msgpack::type::EXT, element.key.type);
          if (strncmp("extracted", element.key.via.ext.data(), element.key.via.ext.size) == 0) {
            EXPECT_EQ(msgpack::type::STR, element.val.type);
            EXPECT_EQ(strncmp("yay", element.val.via.str.ptr, 3), 0);
            EXPECT_EQ(strncmp("yay!!!", element.val.via.str.ptr, 6), 0);
          } else if (strncmp("stdout", element.key.via.ext.data(), element.key.via.ext.size) == 0) {
            EXPECT_EQ(msgpack::type::STR, element.val.type);
            EXPECT_EQ(strncmp("(can't read stdout)", element.val.via.str.ptr, 19), 0);
          } else {
            FAIL();
          }
        }
        break;
      default:
        EXPECT_TRUE(false);
    }
  }
}
