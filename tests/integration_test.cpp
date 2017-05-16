//
// Created by Alex Snaps on 2016-12-05.
//

#include "error.hpp"
#include "script_runner.hpp"
#include "gtest/gtest.h"

static const int BUFSIZE = 1024;

TEST(integration_test, happy_path) {

  script_data script;
  {
    auto file = fopen("../../tests/data/data.mp", "r");
    if (file == NULL) {
      perror("Error");
      FAIL();
    }
    auto fd = fileno(file);
    script.read_from(fd);
    close(fd);
  }

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
    timer t = timer([&writer](const std::string, const int64_t) {});
    script_runner runner(*engine, t);
    runner.run(script, writer);
  }

  close(fd[1]);
  me_mruby_engine_destroy(engine);
  me_memory_pool_destroy(allocator);

  char output[BUFSIZE];
  msgpack::unpacker pac;
  ssize_t r, in = 0;
  while ((r = read(fd[0], &output + in, (size_t) (BUFSIZE - in))) > 0) {
    pac.reserve_buffer(r);
    memcpy(pac.buffer(), &output + in, r);
    pac.buffer_consumed(r);
  }

  msgpack::object_handle oh;
  int elements = 0;
  while (pac.next(oh)) {
    auto object = oh.get();
    switch (++elements) {
      case 1: { // OUR OUTPUT
        EXPECT_EQ(msgpack::type::ARRAY, object.type);
        EXPECT_EQ(uint32_t{2}, object.via.array.size);
        int i = 0;
        for (auto &&item : object.via.array) {
          switch (i++) {
            case 0 :
              EXPECT_EQ(msgpack::type::EXT, item.type);
              ASSERT_EQ(strncmp("output", item.via.ext.data(), 6), 0);
              break;
            case 1:
              EXPECT_EQ(msgpack::type::MAP, item.type);
              EXPECT_EQ(uint32_t{2}, item.via.map.size);
              for (auto &&element: item.via.map) {
                EXPECT_EQ(msgpack::type::EXT, element.key.type);
                if (strncmp("extracted", element.key.via.ext.data(), element.key.via.ext.size) == 0) {
                  EXPECT_EQ(msgpack::type::MAP, element.val.type);
                  EXPECT_EQ(uint32_t{1}, element.val.via.map.size);
                } else if (strncmp("stdout", element.key.via.ext.data(), element.key.via.ext.size) == 0) {
                  EXPECT_EQ(msgpack::type::STR, element.val.type);
                } else {
                  FAIL();
                }
              }
              break;
            default:
              FAIL();
          }
        }
      }
      break;
      case 2: { // OUR STATS
        EXPECT_EQ(msgpack::type::ARRAY, object.type);
        EXPECT_EQ(uint32_t{2}, object.via.array.size);
        int i = 0;
        for (auto &&item : object.via.array) {
          switch (i++) {
            case 0 :
              EXPECT_EQ(msgpack::type::EXT, item.type);
              break;
            case 1:
              EXPECT_EQ(msgpack::type::MAP, item.type);
              break;
            default:
              FAIL();
          }
        }
      }
      break;
      default:
        std::cerr << elements << ": " << object << std::endl;
        FAIL();
    }
  }
  close(fd[0]);
}

TEST(integration_test, large_payload) {

  script_data script;
  {
    auto file = fopen("../../tests/data/large.mp", "r");
    if (file == NULL) {
      perror("Error");
      FAIL();
    }
    auto fd = fileno(file);
    script.read_from(fd);
    close(fd);
  }

  int fd = -1;
  {
    auto file = fopen("/dev/null", "w");
    if (file == NULL) {
      perror("Error");
      FAIL();
    }
    fd = fileno(file);
  }
  output_stream stream{fd};
  out_packer packer{stream};
  data_writer writer(packer);

  me_memory_pool *allocator = me_memory_pool_new(4 * MiB);
  me_mruby_engine *engine = me_mruby_engine_new(allocator, 1000000);

  {
    timer t = timer([&writer](const std::string, const int64_t) {});
    script_runner runner(*engine, t);
    runner.run(script, writer);
  }
}
