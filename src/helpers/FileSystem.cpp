#include "FileSystem.h"

#include <fstream>
#include <iostream>
#include <filesystem>

bool FileSystem::ReadFile(const char *aFilePath, std::vector<char> &someBytes) {
  try {
    std::ifstream inFile{aFilePath, std::ios::binary};
    inFile.exceptions(std::ios::failbit | std::ios::badbit);

    // get the file size and read all bytes
    inFile.seekg(0, std::ios::end);
    size_t fileSize = inFile.tellg();
    inFile.seekg(0, std::ios::beg);

    std::vector<char> bytes(fileSize);
    inFile.read(reinterpret_cast<char *>(bytes.data()), fileSize);
    someBytes = bytes;
    someBytes.push_back('\0');
  }
  catch(const std::exception& e){
    std::cerr << "Unexpected error reading file: " << e.what();
    return false;
  }
  return true;
}
