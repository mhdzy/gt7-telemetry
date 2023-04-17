#pragma once

#include <sys/stat.h>

#include <fstream>
#include <string>
#include <vector>

extern std::string vpaste(std::vector<std::string> fields, std::string sep = ",");
extern std::uint8_t createDir(std::string directory);

extern std::vector<unsigned char> file_read(std::string filename);
extern std::size_t file_size(std::string filename);

extern std::vector<std::vector<unsigned char>> parseBytes(std::vector<std::size_t> sizes,
                                                                  std::vector<unsigned char> filebytes,
                                                                  std::uint16_t offset);
