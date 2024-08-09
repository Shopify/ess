//
// Created by Alex Snaps on 2016-11-30.
//

#include "data.hpp"
#include "gtest/gtest.h"

static const int BUFSIZE = 1024;

TEST(data_writer_test, outputs_measurements) {
  int fd[2];
  if (pipe(fd) == -1) {
    perror("pipe");
    FAIL();
  }

  unsigned char expected[23] = {
      0x92, 0xC7, 0x0B, 0x00,
      0x6D, 0x65, 0x61, 0x73, 0x75, 0x72, 0x65, 0x6D, 0x65, 0x6E, 0x74, // measurement
      0x92, 0xC7, 0x03, 0x00,
      0x66, 0x6F, 0x6F, // foo
      0x2A, // 42
  };
  output_stream stream{fd[1]};
  out_packer packer{stream};
  data_writer writer(packer);
  writer.emit_measurement("foo", 42);
  close(fd[1]);

  char output[BUFSIZE];
  ssize_t r, in = 0;
  while ((r = read(fd[0], output + in, (size_t) (BUFSIZE - in))) > 0) {
    if ((in += r) >= BUFSIZE) break;
  }
  close(fd[0]);

  EXPECT_EQ(23, in);
  EXPECT_EQ(0, memcmp(expected, output, in));
}
