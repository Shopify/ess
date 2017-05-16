//
// Created by Alex Snaps on 2016-11-30.
//

#include <gtest/gtest.h>
#include "data.hpp"
#include "error.hpp"
#include "script_data.hpp"

TEST(script_data_test, fails_on_empty_data) {
  int fd[2];
  if (pipe(fd) == -1) {
    perror("pipe");
    FAIL();
  }

  output_stream stream{fd[1]};
  out_packer packer{stream};
  packer.pack_map(0);
  close(fd[1]);

  script_data script;
  status_code code = status_code::ok;
  try {
    script.read_from(fd[0]);
  } catch (fatal_error e) {
    code = e.get_err_code();
  }
  close(fd[0]);
  EXPECT_EQ(code, status_code::bad_input);
}

TEST(script_data_test, fails_on_invalid_input) {
  int fd[2];
  if (pipe(fd) == -1) {
    perror("pipe");
    FAIL();
  }

  output_stream stream{fd[1]};
  out_packer packer{stream};
  packer.pack_array(0);
  close(fd[1]);

  script_data script;
  status_code code = status_code::ok;
  try {
    script.read_from(fd[0]);
  } catch (fatal_error e) {
    code = e.get_err_code();
  }
  close(fd[0]);
  EXPECT_EQ(code, status_code::bad_input);
}

TEST(script_data_test, fails_on_invalid_sources_type) {
  int fd[2];
  if (pipe(fd) == -1) {
    perror("pipe");
    FAIL();
  }

  output_stream stream{fd[1]};
  out_packer packer{stream};
  packer.pack_map(2);
  packer.pack(symbol{"sources"});
  packer.pack_int64(42);
  packer.pack(symbol{"input"});
  packer.pack_map(0);
  close(fd[1]);

  script_data script;
  status_code code = status_code::ok;
  try {
    script.read_from(fd[0]);
  } catch (fatal_error e) {
    code = e.get_err_code();
  }
  close(fd[0]);
  EXPECT_EQ(code, status_code::bad_input);
}

TEST(script_data_test, fails_on_invalid_sources_length) {
  int fd[2];
  if (pipe(fd) == -1) {
    perror("pipe");
    FAIL();
  }

  output_stream stream{fd[1]};
  out_packer packer{stream};
  packer.pack_map(2);
  packer.pack(symbol{"sources"});
  packer.pack_array(1);
  packer.pack_int64(42);
  packer.pack(symbol{"input"});
  packer.pack_map(0);
  close(fd[1]);

  script_data script;
  status_code code = status_code::ok;
  try {
    script.read_from(fd[0]);
  } catch (fatal_error e) {
    code = e.get_err_code();
  }
  close(fd[0]);
  EXPECT_EQ(code, status_code::bad_input);
}

TEST(script_data_test, fails_on_invalid_sources_path) {
  int fd[2];
  if (pipe(fd) == -1) {
    perror("pipe");
    FAIL();
  }

  output_stream stream{fd[1]};
  out_packer packer{stream};
  packer.pack_map(2);
  packer.pack(symbol{"sources"});
  packer.pack_array(1);
  packer.pack_array(2);
  packer.pack_int32(42);
  packer.pack_str(1);
  packer.pack_str_body("1", 1);
  packer.pack(symbol{"input"});
  packer.pack_false();
  close(fd[1]);

  script_data script;
  status_code code = status_code::ok;
  try {
    script.read_from(fd[0]);
  } catch (fatal_error e) {
    code = e.get_err_code();
  }
  close(fd[0]);
  EXPECT_EQ(code, status_code::bad_input);
}

TEST(script_data_test, fails_on_invalid_sources_source) {
  int fd[2];
  if (pipe(fd) == -1) {
    perror("pipe");
    FAIL();
  }

  output_stream stream{fd[1]};
  out_packer packer{stream};
  packer.pack_map(2);
  packer.pack(symbol{"sources"});
  packer.pack_array(1);
  packer.pack_array(2);
  packer.pack_str(1);
  packer.pack_str_body("1", 1);
  packer.pack(symbol{"input"});
  packer.pack_int32(42);
  packer.pack_false();
  close(fd[1]);

  script_data script;
  status_code code = status_code::ok;
  try {
    script.read_from(fd[0]);
  } catch (fatal_error e) {
    code = e.get_err_code();
  }
  close(fd[0]);
  EXPECT_EQ(code, status_code::bad_input);
}

TEST(script_data_test, fails_on_invalid_library) {
  int fd[2];
  if (pipe(fd) == -1) {
    perror("pipe");
    FAIL();
  }

  output_stream stream{fd[1]};
  out_packer packer{stream};
  packer.pack_map(3);
  packer.pack(symbol{"library"});
  packer.pack_false();
  packer.pack(symbol{"sources"});
  packer.pack_array(1);
  packer.pack_array(2);
  packer.pack_str(1);
  packer.pack_str_body("1", 1);
  packer.pack_str(1);
  packer.pack_str_body("1", 1);
  packer.pack(symbol{"input"});
  packer.pack_false();
  close(fd[1]);

  script_data script;
  status_code code = status_code::ok;
  try {
    script.read_from(fd[0]);
  } catch (fatal_error e) {
    code = e.get_err_code();
  }
  close(fd[0]);
  EXPECT_EQ(code, status_code::bad_input);
}

TEST(script_data_test, ignores_random_data) {
  int fd[2];
  if (pipe(fd) == -1) {
    perror("pipe");
    FAIL();
  }

  output_stream stream{fd[1]};
  out_packer packer{stream};
  packer.pack_map(4);
  packer.pack(symbol{"library"});
  packer.pack_bin(1);
  packer.pack_bin_body("0", 1);
  packer.pack(symbol{"sources"});
  packer.pack_array(1);
  packer.pack_array(2);
  packer.pack_str(1);
  packer.pack_str_body("1", 1);
  packer.pack_str(1);
  packer.pack_str_body("1", 1);
  packer.pack(symbol{"input"});
  packer.pack_false();
  packer.pack(symbol{"random_stuff"});
  packer.pack_true();
  // this is trailing garbage
  packer.pack_false();
  close(fd[1]);

  script_data script;
  status_code code = status_code::ok;
  try {
    script.read_from(fd[0]);
  } catch (fatal_error e) {
    code = e.get_err_code();
  }
  close(fd[0]);
  EXPECT_EQ(code, status_code::ok);
}

TEST(script_data_test, parses_input) {
  int fd[2];
  if (pipe(fd) == -1) {
    perror("pipe");
    FAIL();
  }

  output_stream stream{fd[1]};
  out_packer packer{stream};
  packer.pack_map(3);
  packer.pack(symbol{"library"});
  packer.pack_bin(1);
  packer.pack_bin_body("0", 1);
  packer.pack(symbol{"sources"});
  packer.pack_array(1);
  packer.pack_array(2);
  packer.pack_str(1);
  packer.pack_str_body("1", 1);
  packer.pack_str(1);
  packer.pack_str_body("2", 1);
  packer.pack(symbol{"input"});
  packer.pack_false();
  close(fd[1]);

  script_data script;
  status_code code = status_code::ok;
  try {
    script.read_from(fd[0]);
  } catch (fatal_error e) {
    code = e.get_err_code();
  }
  close(fd[0]);
  EXPECT_EQ(code, status_code::ok);
  EXPECT_EQ(script.size(), std::uint64_t{39});

  me_memory_pool *allocator = me_memory_pool_new(4 * MiB);
  me_mruby_engine *engine = me_mruby_engine_new(allocator, 100000);

  mrb_value value = script.input(*engine);
  EXPECT_TRUE(mrb_type(value) == MRB_TT_FALSE);
  
  auto library = script.library();
  EXPECT_EQ(library.size(), uint64_t{1});
  EXPECT_EQ(library[0], '0');

  auto sources = script.sources();
  EXPECT_EQ(sources.size(), uint64_t{1});
  EXPECT_EQ(sources[0].path, "1");
  EXPECT_EQ(sources[0].source, "2");

  me_mruby_engine_destroy(engine);
  me_memory_pool_destroy(allocator);
}

TEST(script_data_test, fails_when_input_too_deep) {
  int fd[2];
  if (pipe(fd) == -1) {
    perror("pipe");
    FAIL();
  }

  output_stream stream{fd[1]};
  out_packer packer{stream};
  packer.pack_map(3);
  packer.pack(symbol{"library"});
  packer.pack_bin(1);
  packer.pack_bin_body("0", 1);
  packer.pack(symbol{"sources"});
  packer.pack_array(1);
  packer.pack_array(2);
  packer.pack_str(1);
  packer.pack_str_body("1", 1);
  packer.pack_str(1);
  packer.pack_str_body("1", 1);
  packer.pack(symbol{"input"});
  for (int i = 0; i < 64; i++) {
    packer.pack_array(1);
  }
  packer.pack_true();

  close(fd[1]);

  script_data script;
  status_code code = status_code::ok;
  try {
    script.read_from(fd[0]);
  } catch (fatal_error e) {
    code = e.get_err_code();
  }
  close(fd[0]);
  EXPECT_EQ(code, status_code::ok);

  me_memory_pool *allocator = me_memory_pool_new(4 * MiB);
  me_mruby_engine *engine = me_mruby_engine_new(allocator, 100000);

  try {
    script.input(*engine);
  } catch (fatal_error e) {
    code = e.get_err_code();
  }
  me_mruby_engine_destroy(engine);
  me_memory_pool_destroy(allocator);

  EXPECT_EQ(code, status_code::structure_too_deep);
}
