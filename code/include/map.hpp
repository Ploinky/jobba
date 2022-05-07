#include <string>

namespace P3D {
    class Mesh;

    class Map {
    public:
        ~Map();

        void Load(std::string fileName);
    private:
        Mesh* mesh;
        std::string mapName;
    };
}