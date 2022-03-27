#include <string>
#include <vector>
#include <cstddef>

namespace P3D {
    struct file_t {
        unsigned char* data;
        size_t size;
    };

    class Util {
    public:
        static file_t ReadBytesFromFile(std::string file);
    };
}