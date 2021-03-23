#include "config.h"
#include <Lazy.h>
#include <iostream>
#include <string>


float f() {
  return 0.0;
}


int main(int argc, const char *argv[]) {
  if (argc == 2 && std::string(argv[1]) == "--help") {
    std::cout << "lazy " << CPP_VERSION_MAJOR << '.' << CPP_VERSION_MINOR << std::endl;
    std::cout << CMAKE_BUILD_TYPE << std::endl;
  }
  auto obj = cpp::lazy([argc]() {return argc;});
  std::cout << *obj << std::endl;
}