#include <fstream>
#include "util.hpp"
#include "logger.hpp"
#include <cstddef>

namespace P3D {
    file_t Util::ReadBytesFromFile(std::string fileName) {
		unsigned char* f = 0;

		// open the file
		std::ifstream file(fileName, std::ios::in | std::ios::binary | std::ios::ate);

		// if open was successful
		if (file.is_open())
		{
			// find the length of the file
			size_t length = file.tellg();
            
			// collect the file data
			f = new unsigned char[length];
			file.seekg(0, std::ios::beg);
			file.read(reinterpret_cast<char*>(f), length);
			file.close();
		    
            return {f, length};
		}

        Logger::Err("Could not load shader file!");
        return {0, 0};
    }
}