#pragma once
#include <string>
#include <vector>

std::string gpath;
std::vector<std::string> gargv;
extern "C" int execv(const char *path, char *const argv[]) {
  gpath = path;
  gargv.clear();
  for (; argv[0] != nullptr; ++argv)
    gargv.push_back(argv[0]);
  return 0;
}
