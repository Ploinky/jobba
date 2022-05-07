#pragma once

// Structure for a single vertex
namespace P3D {
    class Vertex {
        public:
            float position[3];
            float normal[3];
            float color[4];
            float texCoord[2];
    };
}