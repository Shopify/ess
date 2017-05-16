#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include "gtest/gtest.h"
#include "options.hpp"

static const std::uint64_t DEFAULT_INSTRUCTION_QUOTA = 100000;

TEST(options_test, returns_default_quota_with_no_args) {
  int argc = 1;
  char *argv[] = { (char*) "options_test" }; 

  std::ostringstream os;

  options opts;
  opts.read_from(argc, argv, os);
  uint64_t instruction_quota = opts.instruction_quota();

  EXPECT_TRUE(os.str().empty());
  EXPECT_EQ(DEFAULT_INSTRUCTION_QUOTA, instruction_quota);
}

TEST(options_test, returns_default_quota_start_with_no_args) {
  int argc = 1;
  char *argv[] = { (char*) "options_test" };

  std::ostringstream os;

  options opts;
  opts.read_from(argc, argv, os);
  uint64_t index = opts.instruction_quota_start();

  EXPECT_TRUE(os.str().empty());
  EXPECT_EQ(uint32_t{0}, index);
}

TEST(options_test, returns_default_quota_with_invalid_parameter) {
  int argc = 2;
  char *argv[] = { (char*) "options_test", (char *) "-d 10" }; 

  std::ostringstream os;

  options opts;
  opts.read_from(argc, argv, os);
  uint64_t instruction_quota = opts.instruction_quota();

  // Note; Getopt will output parameter errors to stderr. 
  EXPECT_TRUE(os.str().empty());
  EXPECT_EQ(DEFAULT_INSTRUCTION_QUOTA, instruction_quota);
}

TEST(options_test, returns_default_quota_with_invalid_quota) {
  int argc = 2;
  char *argv[] = { (char*) "options_test", (char *) "-i unicorn" }; 

  std::ostringstream os;

  options opts;
  opts.read_from(argc, argv, os);
  uint64_t instruction_quota = opts.instruction_quota();

  EXPECT_TRUE(os.str().find("invalid argument") != std::string::npos);
  EXPECT_EQ(DEFAULT_INSTRUCTION_QUOTA, instruction_quota);
}

TEST(options_test, returns_default_quota_with_out_of_range_quota) {
  int argc = 2;
  char *argv[] = { (char*) "options_test", (char *) "-i 90000000000000000000" };

  std::ostringstream os;

  options opts;
  opts.read_from(argc, argv, os);
  uint64_t instruction_quota = opts.instruction_quota();

  EXPECT_TRUE(os.str().find("out of range") != std::string::npos);
  EXPECT_EQ(DEFAULT_INSTRUCTION_QUOTA, instruction_quota);
}

TEST(options_test, returns_minimum_quota) {
  uint64_t configured_instruction_quota= 10;
  uint64_t minimum_instruction_quota = 6000;

  char arg_value[100];
  snprintf(arg_value, 100, "-i %llu", configured_instruction_quota);

  int argc = 2;
  char *argv[] = { (char*) "options_test", arg_value }; 

  std::ostringstream os;

  options opts;
  opts.read_from(argc, argv, os);
  uint64_t instruction_quota = opts.instruction_quota();

  EXPECT_TRUE(os.str().empty());
  EXPECT_EQ(minimum_instruction_quota, instruction_quota);
}

TEST(options_test, returns_configured_quota) {
  uint64_t configured_instruction_quota = rand() % UINT64_MAX;

  char arg_value[100];
  snprintf(arg_value, 100, "-i %llu", configured_instruction_quota);

  int argc = 2;
  char *argv[] = { (char*) "options_test", arg_value }; 

  std::ostringstream os;

  options opts;
  opts.read_from(argc, argv, os);
  uint64_t instruction_quota = opts.instruction_quota();

  EXPECT_TRUE(os.str().empty());
  EXPECT_EQ(configured_instruction_quota, instruction_quota);
}

TEST(options_test, returns_configured_quota_start) {
  uint32_t configured_start = rand() % UINT32_MAX;

  char arg_value[100];
  snprintf(arg_value, 100, "-C %u", configured_start);

  int argc = 2;
  char *argv[] = { (char*) "options_test", arg_value };

  std::ostringstream os;

  options opts;
  opts.read_from(argc, argv, os);

  EXPECT_TRUE(os.str().empty());
  EXPECT_EQ(configured_start, opts.instruction_quota_start());
}

TEST(options_test, returns_configured_quota_start_maxed) {

  char arg_value[100];
  snprintf(arg_value, 100, "-C %llu", UINT64_MAX);

  int argc = 2;
  char *argv[] = { (char*) "options_test", arg_value };

  std::ostringstream os;

  options opts;
  opts.read_from(argc, argv, os);

  EXPECT_TRUE(os.str().empty());
  EXPECT_EQ(UINT32_MAX, opts.instruction_quota_start());
}

TEST(options_test, parses_all_options) {

  char *opt1 = (char *) "-C";
  char *val1 = (char *) "42";
  char *opt2 = (char *) "-i";
  char *val2 = (char *) "100200";

  int argc = 5;
  char *argv[] = { (char *) "options_test", opt1, val1, opt2, val2 };

  options opts;
  opts.read_from(argc, argv);

  EXPECT_EQ(uint32_t{42}, opts.instruction_quota_start());
  EXPECT_EQ(uint64_t{100200}, opts.instruction_quota());
}
