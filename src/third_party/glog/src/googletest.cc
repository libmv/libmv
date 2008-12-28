#include "third_party/gflags/gflags.h"

DEFINE_string(test_tmpdir, "/tmp", "Dir we use for temp files");
DEFINE_string(test_srcdir, "../../../../src/third_party/glog",
              "Source-dir root, needed to find glog_unittest_flagfile");
DEFINE_int32(benchmark_iters, 100000000, "Number of iterations per benchmark");
