#ifndef ENTERPRISE_SCRIPT_SERVICE_DLMALLOC_CONFIG_H
#define ENTERPRISE_SCRIPT_SERVICE_DLMALLOC_CONFIG_H

#include "error.hpp"

#define ONLY_MSPACES 1
#define HAVE_MREMAP 0

#define CORRUPTION_ERROR_ACTION(state)                                  \
  do {                                                                  \
    leave(status_code::memory_corruption);                               \
  } while(0)

#define USAGE_ERROR_ACTION(state, chunk)                                \
  do {                                                                  \
    leave(status_code::memory_usage_error);                              \
  } while(0)

#endif
