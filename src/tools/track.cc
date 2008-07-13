#include "third_party/gflags/gflags.h"

// This is an example of how to use the flags facilities.

DEFINE_int32(news_at, 11, "Explanation of option here.");

int main(int argc, char **argv) {
 google::SetUsageMessage("track a sequence.");
 google::ParseCommandLineFlags(&argc, &argv, true);

 return 0;
}


