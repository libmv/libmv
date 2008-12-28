ADD_LIBRARY(glog
            src/logging.cc
            src/demangle.cc
            src/symbolize.cc
            src/utilities.cc
            src/raw_logging.cc
            src/vlog_is_on.cc
)

# uinttests
#           src/logging_striptest_main.cc
#           src/logging_striptest2.cc
#           src/signalhandler.cc
#           src/utilities_unittest.cc
#           src/signalhandler_unittest.cc
#           src/logging_unittest.cc
#           src/stl_logging_unittest.cc
#           src/stacktrace_unittest.cc
#           src/logging_striptest10.cc
#           src/demangle_unittest.cc
#           src/symbolize_unittest.cc
