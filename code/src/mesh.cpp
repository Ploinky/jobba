#include "mesh.hpp"
#include "vertex.hpp"
#include "direct3d.hpp"
#include "DDSTextureLoader.hpp"

namespace P3D {
    Mesh::~Mesh() {
        vertexBuffer->Release();
        indexBuffer->Release();
    }

    bool Mesh::Initialize(Direct3D* direct3D) {
        vertexBuffer = direct3D->CreateVertexBuffer(vertices, vertexCount);

        if(vertexBuffer == nullptr) {
            initialized = false;
            return false;
        }

        indexBuffer = direct3D->CreateIndexBuffer(indices, indexCount);

        if(indexBuffer == nullptr) {
            initialized = false;
            return false;
        }

        LoadTexture(direct3D, L"./data/maps/map1/metallic.dds", &metallic);
        LoadTexture(direct3D, L"./data/maps/map1/normal.dds", &normal);
        LoadTexture(direct3D, L"./data/maps/map1/roughness.dds", &roughness);

        initialized = true;

        return initialized;
    }

    bool Mesh::IsInitialized() {
        return initialized;
    }

    void Mesh::LoadTexture(Direct3D* direct3D, std::wstring fileName, texture_t* tex) {
        // Load the texture in.
        ID3D11Resource* texture;
        const HRESULT result = DirectX::CreateDDSTextureFromFile(direct3D->device, fileName.c_str(), &texture, &tex->shaderResourceView);
        if (FAILED(result)) {
            MessageBoxW(NULL, L"ouch", L"YIKES", MB_ICONERROR);
            return;
        }

        tex->texture = static_cast<ID3D11Texture2D*>(texture);
    }
}