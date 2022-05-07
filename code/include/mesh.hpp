#pragma once

#include "d3d11.h"
#include <stdint.h>
#include <DirectXMath.h>
#include <string>

namespace P3D {
    class Direct3D;
    class Vector3D;
    class Vertex;

    struct texture_t {
        ID3D11Texture2D* texture;
        ID3D11ShaderResourceView* shaderResourceView;
    };

    class Mesh {
        public:
            Vertex* vertices;
            uint32_t vertexCount;
            unsigned int* indices;
            uint32_t indexCount;
            ID3D11Buffer* vertexBuffer;
            ID3D11Buffer* indexBuffer;
            DirectX::XMFLOAT3 position;
            DirectX::XMFLOAT3 rotation = DirectX::XMFLOAT3();
            texture_t albedo;
            texture_t normal;
            texture_t metallic;
            texture_t roughness;

            ~Mesh();
            bool Initialize(Direct3D* direct3D);
            bool IsInitialized();

        private:
            bool initialized = false;
            void LoadTexture(Direct3D* direct3D, std::wstring fileName, texture_t* texture);
    };
}