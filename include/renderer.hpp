#include <d3d11.h>
#include <DirectXMath.h>

namespace P3D {
    class Model3D;
    class Direct3D;

    struct constant_buffer {
        DirectX::XMFLOAT4X4 projMatrix;
    };

    class Renderer {
    public:
        void Initialize(Direct3D* direct3D);
        void Render(Model3D* model);

        Direct3D* direct3D;
        ID3D11InputLayout* inputLayout;
        ID3D11VertexShader* vertexShader;
        ID3D11PixelShader* pixelShader;
        ID3D11Buffer* constantBuffer;
        constant_buffer vsConstData;
    };
}