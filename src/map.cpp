#include "map.hpp"
#include "mesh.hpp"
#include "util.hpp"
#include "logger.hpp"
namespace P3D {
    Map::~Map() {
        delete mesh;
    }

    void Map::Load(std::string fileName) {
        // Plain old data lump...
        file_t mapData = Util::ReadBytesFromFile(fileName);

        if(mapData.data[0] != 'o' || mapData.data[1] != 'm' || mapData.data[2] != 'p') {
            Logger::Err("Magic string at beginning of map file not found.");
        } 
    }
}