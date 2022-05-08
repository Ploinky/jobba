#include "renderer.hpp"
#include "mesh.hpp"
#include "logger.hpp"
#include "vertex.hpp"
#include "direct3d.hpp"
#include "util.hpp"
#include "camera.hpp"

namespace P3D {
    static DirectX::XMMATRIX perspMatrix;

    Renderer::~Renderer() {
        inputLayout->Release();
        vertexShader->Release();
        pixelShader->Release();
        constantBuffer->Release();
        frameConstantBuffer->Release();
        sampler->Release();
    }

    void Renderer::Initialize(Direct3D* direct3D, int width, int height) {
        this->direct3D = direct3D;
        camera = new Camera();

        perspMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(
            DirectX::XMConvertToRadians(camera->fov), width / height, camera->nearClip, camera->farClip));

        // Where to set this?
        direct3D->context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // Describes the input layout
        D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
        };

        file_t shaderByteCode = Util::ReadBytesFromFile("./shaders/vertex.cso");
        file_t psByteCode = Util::ReadBytesFromFile("./shaders/pixel.cso");

        direct3D->device->CreateVertexShader(shaderByteCode.data, shaderByteCode.size, nullptr, &vertexShader);
        direct3D->device->CreatePixelShader(psByteCode.data, psByteCode.size, nullptr, &pixelShader);

        direct3D->device->CreateInputLayout(inputLayoutDesc, 3, shaderByteCode.data, shaderByteCode.size, &inputLayout);

        DirectX::XMStoreFloat4x4(&frameConstBuffer.projMatrix, perspMatrix);
        DirectX::XMStoreFloat4x4(&frameConstBuffer.cameraMatrix, DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixTranslation(
            camera->position.x, camera->position.y, camera->position.z))));
        DirectX::XMStoreFloat4x4(&vsConstData.modelMatrix, DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslation(0, 0, 10)));
        
        point_light_t light;
        light.position = DirectX::XMFLOAT4(-3.0f, 2.0f, 0.0f, 0.0f);
        light.color = DirectX::XMFLOAT4(0.7f, 0.7f, 0.7f, 0.0f);
        frameConstBuffer.pointLights[0] = light;
        
        D3D11_BUFFER_DESC desc;
        desc.ByteWidth = sizeof(object_constant_buffer);
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA data;
        data.pSysMem = &vsConstData;

        direct3D->device->CreateBuffer(&desc, &data, &constantBuffer);
        
        desc.ByteWidth = sizeof(frame_constant_buffer);
        desc.Usage = D3D11_USAGE_DYNAMIC;

        data.pSysMem = &frameConstBuffer;

        direct3D->device->CreateBuffer(&desc, &data, &frameConstantBuffer);
        
        D3D11_SAMPLER_DESC samplerDesc;
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.MipLODBias = 0.0f;
        samplerDesc.MaxAnisotropy = 1;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        samplerDesc.BorderColor[0] = 0;
        samplerDesc.BorderColor[1] = 0;
        samplerDesc.BorderColor[2] = 0;
        samplerDesc.BorderColor[3] = 0;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

        HRESULT hr = direct3D->device->CreateSamplerState(&samplerDesc, &sampler);
        if (FAILED(hr)) {
            MessageBoxW(NULL, L"ouch", L"YIKES", MB_ICONERROR);
            return;
        }
    }

    void Renderer::Render(Mesh* model) {
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

        DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(model->rotation.x),
                        DirectX::XMConvertToRadians(model->rotation.y),
                        DirectX::XMConvertToRadians(model->rotation.z));
        
        DirectX::XMMATRIX transMat =  DirectX::XMMatrixTranslation(model->position.x, model->position.y, model->position.z);

        DirectX::XMStoreFloat4x4(&vsConstData.modelMatrix, DirectX::XMMatrixTranspose(rotMat * transMat));

        UpdateModelConstantBuffer();

        direct3D->context->VSSetShader(vertexShader, 0, 0);
        direct3D->context->VSSetConstantBuffers(0, 1, &frameConstantBuffer);
        direct3D->context->VSSetConstantBuffers(1, 1, &constantBuffer);
        direct3D->context->PSSetShader(pixelShader, 0, 0);
        direct3D->context->PSSetSamplers(0, 1, &sampler);
	    direct3D->context->PSSetConstantBuffers(0, 1, &frameConstantBuffer);
        direct3D->context->PSSetShaderResources(0, 1, &model->albedo.shaderResourceView);
        direct3D->context->PSSetShaderResources(1, 1, &model->normal.shaderResourceView);
        direct3D->context->PSSetShaderResources(2, 1, &model->metallic.shaderResourceView);
        direct3D->context->PSSetShaderResources(3, 1, &model->roughness.shaderResourceView);
        direct3D->context->IASetInputLayout(inputLayout);
      
        direct3D->context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        direct3D->context->IASetVertexBuffers(0, 1, &model->vertexBuffer, &stride, &offset);
        direct3D->context->IASetIndexBuffer(model->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

        direct3D->context->DrawIndexed(model->indexCount, 0, 0);
    }

    void Renderer::SetAspectRatio(float aspect) {
        perspMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(camera->fov), aspect, camera->nearClip, camera->farClip));
        DirectX::XMStoreFloat4x4(&frameConstBuffer.projMatrix, perspMatrix);

        UpdateFrameConstantBuffer();
    }

    void Renderer::UpdateCameraMatrix() {
        DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(camera->rotation.x),
                        DirectX::XMConvertToRadians(camera->rotation.y),
                        DirectX::XMConvertToRadians(camera->rotation.z));
        
        DirectX::XMMATRIX transMat =  DirectX::XMMatrixTranslation(camera->position.x, camera->position.y, camera->position.z);

        DirectX::XMStoreFloat4x4(&frameConstBuffer.cameraMatrix, DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, rotMat * transMat)));

        frameConstBuffer.camPos = DirectX::XMFLOAT4(camera->position.x, camera->position.y, camera->position.z, 0.0f);

        UpdateFrameConstantBuffer();
    }

    void Renderer::UpdateFrameConstantBuffer() {
        D3D11_MAPPED_SUBRESOURCE mappedResource = {0};

        direct3D->context->Map(frameConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

        memcpy(mappedResource.pData, &frameConstBuffer, sizeof(frameConstBuffer));

        direct3D->context->Unmap(frameConstantBuffer, 0);
    }

    void Renderer::UpdateModelConstantBuffer() {        
        D3D11_MAPPED_SUBRESOURCE mappedResource = {0};

        direct3D->context->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

        memcpy(mappedResource.pData, &vsConstData, sizeof(vsConstData));

        direct3D->context->Unmap(constantBuffer, 0);
    }
}