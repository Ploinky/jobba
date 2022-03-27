#include "renderer.hpp"
#include "model3d.hpp"
#include "logger.hpp"
#include "vertex.hpp"
#include "direct3d.hpp"
#include "util.hpp"

namespace P3D {
    static DirectX::XMMATRIX perspMatrix;

    void Renderer::Initialize(Direct3D* direct3D) {
        this->direct3D = direct3D;

        perspMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(70), 1 / 1, 0.0001f, 1000);

        // Where to set this?
        direct3D->context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // Describes the input layout
        D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
        };

        file_t shaderByteCode = Util::ReadBytesFromFile("D:/Projects/jobba/build/shaders/vertex.cso");
        file_t psByteCode = Util::ReadBytesFromFile("D:/Projects/jobba/build/shaders/pixel.cso");

        direct3D->device->CreateVertexShader(shaderByteCode.data, shaderByteCode.size, nullptr, &vertexShader);
        direct3D->device->CreatePixelShader(psByteCode.data, psByteCode.size, nullptr, &pixelShader);

        direct3D->device->CreateInputLayout(inputLayoutDesc, 2, shaderByteCode.data, shaderByteCode.size, &inputLayout);

        DirectX::XMStoreFloat4x4(&vsConstData.projMatrix, perspMatrix);
        
        D3D11_BUFFER_DESC desc;
        desc.ByteWidth = sizeof(constant_buffer);
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA data;
        data.pSysMem = &vsConstData;
        data.SysMemPitch = 0;
        data.SysMemSlicePitch = 0;

        direct3D->device->CreateBuffer(&desc, &data, &constantBuffer);
    }

    void Renderer::Render(Model3D* model) {
        // Lazy initialize the model's Direct3D resources
        if(!model->IsInitialized()) {
            if(!model->Initialize(direct3D)) {
                Logger::Err("Failed to initialize 3D model!");
                // Maybe quit here?
                return;
            }
        }

        // Model is definitely initialized, feel free to render!

        UINT stride = sizeof(Vertex);
        UINT offset = 0;

        direct3D->context->VSSetShader(vertexShader, 0, 0);
        direct3D->context->VSSetConstantBuffers(0, 1, &constantBuffer);
        direct3D->context->PSSetShader(pixelShader, 0, 0);
        direct3D->context->IASetInputLayout(inputLayout);
      
        direct3D->context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        direct3D->context->IASetVertexBuffers(0, 1, &model->vertexBuffer, &stride, &offset);
        direct3D->context->IASetIndexBuffer(model->indexBuffer, DXGI_FORMAT_R16_UINT, 0);

        direct3D->context->Draw(model->indexCount, 0);
    }
}