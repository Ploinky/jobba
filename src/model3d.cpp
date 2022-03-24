#include "model3d.hpp"
#include "vertex.hpp"
#include "direct3d.hpp"

namespace P3D {

    bool Model3D::Initialize(Direct3D* direct3D) {
        initialized = false;

        vertexBuffer = direct3D->CreateVertexBuffer(vertices, vertexCount);

        if(vertexBuffer == nullptr) {
            return initialized;
        }

        unsigned int* indices = new unsigned int[vertexCount];

        // This is very naive, no?
        for(int i = 0; i < vertexCount; i++) {
            indices[i] = i;
        }

        indexBuffer = direct3D->CreateIndexBuffer(indices, vertexCount);

        initialized = true;

        return initialized;
    }

    bool Model3D::IsInitialized() {
        return initialized;
    }
}