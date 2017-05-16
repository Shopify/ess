#include "sandbox.hpp"

#ifdef __linux__

#include "error.hpp"
#include "units.hpp"
#include <algorithm>
#include <climits>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdio.h>
#include <unistd.h>
#include <seccomp.h>
#include <sys/prctl.h>
#include <sys/resource.h>
#include <linux/seccomp.h>

void reserve_memory() {
  mallopt(M_TRIM_THRESHOLD, 64 * MiB);
  free(malloc(32 * MiB));
}

static void check_seccomp(int result) {
  if (result != 0) {
    leave(status_code::bad_seccomp_filter);
  }
}

void sandbox() {


  auto context = seccomp_init(SCMP_ACT_KILL);
  if (context == nullptr) {
    leave(status_code::bad_seccomp_filter);
  }

  check_seccomp(seccomp_rule_add_exact(
    context, SCMP_ACT_ALLOW, SCMP_SYS(exit), 0));
  check_seccomp(seccomp_rule_add_exact(
    context, SCMP_ACT_ALLOW, SCMP_SYS(read), 1,
    SCMP_A0(SCMP_CMP_EQ, STDIN_FILENO)));
  check_seccomp(seccomp_rule_add_exact(
    context, SCMP_ACT_ALLOW, SCMP_SYS(write), 1,
    SCMP_A0(SCMP_CMP_EQ, STDOUT_FILENO)));
  check_seccomp(seccomp_rule_add_exact(
    context, SCMP_ACT_ALLOW, SCMP_SYS(write), 1,
    SCMP_A0(SCMP_CMP_EQ, STDERR_FILENO)));

  check_seccomp(seccomp_load(context));
  seccomp_release(context);
}

#else

void reserve_memory() {}
void sandbox() {}

#endif
