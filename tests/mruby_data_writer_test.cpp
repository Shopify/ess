//
// Created by Alex Snaps on 2016-11-30.
//

#include "script_runner.hpp"
#include <gtest/gtest.h>
#include <error.hpp>

static const int BUFSIZE = 1024;

TEST(mruby_data_writer_test, outputs_valid_data) {
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

  {
    mruby_data_writer engine_writer(writer, *engine);
    engine_writer.emit_output();
  }
  close(fd[1]);
  me_mruby_engine_destroy(engine);
  me_memory_pool_destroy(allocator);

  char output[BUFSIZE];
  ssize_t r, in = 0;
  while ((r = read(fd[0], &output + in, (size_t) (BUFSIZE - in))) > 0) {
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
            EXPECT_EQ(msgpack::type::NIL, element.val.type);
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

TEST(mruby_data_writer_test, throws_on_too_deep_stack) {
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

  auto src = ruby_source{"A", "@output = [[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[false]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]"};
  auto pProc = engine->generate_code(src);
  engine->eval(pProc);

  status_code code = status_code::ok;
  {
    mruby_data_writer engine_writer(writer, *engine);
    try {
      engine_writer.emit_output();
    } catch (fatal_error e) {
      code = e.get_err_code();
    }
  }
  close(fd[1]);
  me_mruby_engine_destroy(engine);
  me_memory_pool_destroy(allocator);

  EXPECT_EQ(code, status_code::structure_too_deep);

  char output[BUFSIZE];
  ssize_t r, in = 0;
  while ((r = read(fd[0], &output + in, (size_t) (BUFSIZE - in))) > 0) {
    if ((in += r) >= BUFSIZE) break;
  }

  msgpack::object_handle oh = msgpack::object_handle();
  ASSERT_THROW(msgpack::unpack(output, in), std::exception);
  close(fd[0]);
}

TEST(mruby_data_writer_test, extracts_data) {
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

  auto source = ruby_source{"A", "@output = [false, true]\n"
      "@stdout_buffer = 'yay!'\n"};
  auto pProc = engine->generate_code(source);
  engine->eval(pProc);

  {
    mruby_data_writer engine_writer(writer, *engine);
    engine_writer.emit_output();
  }
  close(fd[1]);
  me_mruby_engine_destroy(engine);
  me_memory_pool_destroy(allocator);

  char output[BUFSIZE];
  ssize_t r, in = 0;
  while ((r = read(fd[0], &output + in, (size_t) (BUFSIZE - in))) > 0) {
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
            EXPECT_EQ(msgpack::type::ARRAY, element.val.type);
            EXPECT_EQ(uint32_t{2}, element.val.via.array.size);
          } else if (strncmp("stdout", element.key.via.ext.data(), element.key.via.ext.size) == 0) {
            EXPECT_EQ(msgpack::type::STR, element.val.type);
            EXPECT_EQ(strncmp("yay!", element.val.via.str.ptr, 4), 0);
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

TEST(mruby_data_writer_test, emits_stat) {
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

  auto source = ruby_source{"A", "@output = [false, true]\n"
      "@stdout_buffer = 'yay!'\n"};
  auto pProc = engine->generate_code(source);
  engine->eval(pProc);

  {
    mruby_data_writer engine_writer(writer, *engine, 42);
  }
  close(fd[1]);
  me_mruby_engine_destroy(engine);
  me_memory_pool_destroy(allocator);

  char output[BUFSIZE];
  ssize_t r, in = 0;
  while ((r = read(fd[0], &output + in, (size_t) (BUFSIZE - in))) > 0) {
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
        EXPECT_EQ(uint32_t{4}, item.via.ext.size);
        EXPECT_EQ(0, strncmp("stat", item.via.ext.data(), 4));
        break;
      case 1:
        EXPECT_EQ(msgpack::type::MAP, item.type);
        EXPECT_EQ(uint32_t{4}, item.via.map.size);
        for (auto && element: item.via.map) {
          EXPECT_EQ(msgpack::type::EXT, element.key.type);
          if (strncmp("instructions", element.key.via.ext.data(), element.key.via.ext.size) == 0) {
            EXPECT_EQ(msgpack::type::POSITIVE_INTEGER, element.val.type);
          } else if (strncmp("total_instructions", element.key.via.ext.data(), element.key.via.ext.size) == 0) {
            EXPECT_EQ(msgpack::type::POSITIVE_INTEGER, element.val.type);
          } else if (strncmp("memory", element.key.via.ext.data(), element.key.via.ext.size) == 0) {
            EXPECT_EQ(msgpack::type::POSITIVE_INTEGER, element.val.type);
          } else if (strncmp("bytes_in", element.key.via.ext.data(), element.key.via.ext.size) == 0) {
            EXPECT_EQ(msgpack::type::POSITIVE_INTEGER, element.val.type);
            EXPECT_EQ(std::uint64_t{42}, element.val.via.u64);
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

