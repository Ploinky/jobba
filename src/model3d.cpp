#include "model3d.hpp"
#include "vertex.hpp"
#include "direct3d.hpp"

namespace P3D {

    bool Model3D::Initialize(Direct3D* direct3D) {
        position.x = 0;
        position.y = 0;
        position.z = 10;

        vertexBuffer = direct3D->CreateVertexBuffer(vertices, vertexCount);

        if(vertexBuffer == nullptr) {
            initialized = false;
            return false;
        }

        indices = new unsigned int[vertexCount];
        indexCount = vertexCount;

        // This is very naive, no?
        for(int i = 0; i < vertexCount; i++) {
            indices[i] = i;
        }

        indexBuffer = direct3D->CreateIndexBuffer(indices, indexCount);

        if(indexBuffer == nullptr) {
            initialized = false;
            return false;
        }

        initialized = true;

        return initialized;
    }

    bool Model3D::IsInitialized() {
        return initialized;
    }
}