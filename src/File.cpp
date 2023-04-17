#include "../include/File.hpp"

/*
  Formats a csv-style header, configurable by 'sep'. Can also be used to
  format any generic set of string fields by any separator string.

  @param fields a vector of fieldnames to include in the header
  @param sep a string separator, defaults to ","
  @return a string whose contents is the fields separated by 'sep'
*/
std::string vpaste(std::vector<std::string> fields, std::string sep) {
  std::string str;

  for (std::string s : fields) str += s + sep;

  // remove final 'sep' character from string
  if (!str.empty()) str.pop_back();

  return str;
}

/**
 * @brief Create a Dir if it does not exist
 * 
 * @param directory 
 * @return std::uint8_t 0 if successful, 1 if exists, -1 if fails
 */
std::uint8_t createDir(std::string directory) {
  // from <sys/stat.h>
  if (mkdir(directory.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
    if (errno == EEXIST) {
      return 1;
    } else {
      return -1;
    }
  } else {
    return 0;
  }
}

/*
  Reads in file bytes into a vector of unsigned chars (1 byte per entry).

  @param filename a file whose contents will be read byte-by-byte
  @return a vector of bytes (stored as unsigned chars)
*/
std::vector<unsigned char> file_read(std::string filename) {
  std::streampos size = file_size(filename);
  std::ifstream file(filename, std::ios::binary);
  std::vector<unsigned char> file_data(size);

  file.read((char *)&file_data[0], size);
  file.close();

  return file_data;
}

/*
  Calculates the size of a file (in bytes).

  @param filename a file whose size will be returned
  @param the file's size
*/
std::size_t file_size(std::string filename) {
  std::streampos size;
  std::ifstream file(filename, std::ios::binary);

  file.seekg(0, std::ios::end);
  size = file.tellg();
  // file.seekg(0, std::ios::beg);
  file.close();

  return size;
}

/**
 * @brief Parses a vector of raw bytes (unsigned char) into a vector of items.
 *
 * Each item in the resulting vector will have a size dictated by its position
 * relative to the `sizes` input vector. `sizes` also dictates how the `filebytes`
 * vector will be split (shifted by `offset`).
 *
 * @param sizes a vector of item sizes
 * @param filebytes a vector of raw bytes to split
 * @param offset an offset to begin parsing the bytes at (files are < 65 kb)
 * @return std::vector<std::vector<unsigned char>> each element being a vector of bytes representing an item.
 */
std::vector<std::vector<unsigned char>> parseBytes(std::vector<std::size_t> sizes,
                                                           std::vector<unsigned char> filebytes,
                                                           std::uint16_t offset = 0) {
  std::vector<std::vector<unsigned char>> pbytes;

  for (std::size_t size : sizes) {
    std::vector<unsigned char> bytes;
    std::vector<unsigned char> tbytes;

    tbytes = {filebytes.begin() + offset, filebytes.begin() + offset + size};
    std::copy(tbytes.begin(), tbytes.end() + 1, std::back_inserter(bytes));
    pbytes.push_back(bytes);

    offset = offset + size;
  }

  return pbytes;
}
