#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <stdexcept>
#include "options.hpp"
#include "units.hpp"

static const std::uint64_t DEFAULT_INSTRUCTION_QUOTA = 100000;
static const std::uint64_t MIN_INSTRUCTION_QUOTA = 6000;
static const std::size_t DEFAULT_MEMORY_QUOTA = 8 * MiB;
 
void options::read_from(int argc, char **argv, std::ostream &output) {
  int opt;
  while ((opt = getopt(argc, argv, "i:C:m:")) != -1) {
    switch(opt) {
      case 'i':
        parse(output, this->instruction_quota_, "instruction quota (-i)");
        if (this->instruction_quota_ < MIN_INSTRUCTION_QUOTA) {
          this->instruction_quota_ = MIN_INSTRUCTION_QUOTA;
        }
        break;
      case 'C': {
        uint64_t value;
        parse(output, value, "instruction quota start (-C)");
        this->instruction_quota_start_ = (uint32_t) (value < UINT32_MAX ? value : UINT32_MAX);
        break;
      }
      case 'm': {
        uint64_t value;
        parse(output, value, "memory quota (-m)");
        this->memory_quota_ = (size_t) (value < SIZE_MAX ? value : SIZE_MAX);
        break;
      }
      default: ; // noop
    }
  }

  optind = 1; // In case we call getopt multiple times like in testing

  // Note; Getopt will output parameter errors to stderr. 
  std::flush(output);
}

void options::parse(std::ostream &output, uint64_t &to, const std::string &option) {
  try {
    to = std::stoull(optarg);
  } catch (std::invalid_argument e) {
    output << "Could not parse " << option << " from: " << optarg << " (invalid argument)" << "\n";
  } catch (std::out_of_range e) {
    output << "Could not parse " << option << " from: " << optarg << " (out of range)" << "\n";
  }
}

options::options() {
  memory_quota_ = DEFAULT_MEMORY_QUOTA;
  instruction_quota_ = DEFAULT_INSTRUCTION_QUOTA;
  instruction_quota_start_ = 0;
}

uint64_t options::instruction_quota() {
  return instruction_quota_;
}

uint32_t options::instruction_quota_start() {
  return instruction_quota_start_;
}

size_t options::memory_quota() {
  return memory_quota_;
}
