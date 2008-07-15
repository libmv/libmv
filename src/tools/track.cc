#include "third_party/gflags/gflags.h"

// This is an example of how to use the flags facilities.

DEFINE_int32(first_index, 0, "Index of the first image.");
DEFINE_int32(last_index, -1, "Index of the last image.  Use -1 to autodetect it.");

int main(int argc, char **argv) {
  google::SetUsageMessage("Track a sequence.");
  google::ParseCommandLineFlags(&argc, &argv, true);

  printf("argc = %d\n", argc);
  printf("%d\n", FLAGS_first_index);
  printf("%d\n", FLAGS_last_index);

  return 0;
}


