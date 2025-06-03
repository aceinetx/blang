#pragma once
#include "result.hh"

#define argsShift(...) _argsShift(&argc, &argv)

std::string _argsShift(int *argc, char ***argv);
Result<std::string, std::string> readFile(const std::string &filename);
