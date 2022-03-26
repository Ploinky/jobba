#include <fstream>
#include "util.hpp"

namespace P3D {
    file_t Util::ReadBytesFromFile(std::string fileName) {
        std::ifstream file(fileName);

        file.seekg(0, std::ios::end);

        size_t length = file.tellg();

        file.seekg(0, std::ios::beg);

        char* buff = new char[length];

        file.read(buff, length);

        return {buff, length};
    }
}