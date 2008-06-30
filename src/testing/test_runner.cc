// Copyright 2008 Keir Mierle, all rights reserved.
//
// A simple test runner, which can be linked in with any number of test modules
// which all have their own TEST() {...} functions. The TEST() functions are
// automatically registered and run.

#include <iostream>

#include "testing/testing.h"

TEST_REGISTRY;

int main() {
  testsoon::default_reporter rep(std::cout);
  return !testsoon::tests().run(rep);
}
