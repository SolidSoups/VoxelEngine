#pragma once

#include <vector>

class FileSystem {
public:
  static bool ReadFile(const char* aFilePath, std::vector<char>& someBytes);
};
