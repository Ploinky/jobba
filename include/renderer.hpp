#include <d3d11.h>

namespace P3D {
    class Model3D;
    class Direct3D;

    class Renderer {
    public:
        void Initialize(Direct3D* direct3D);
        void Render(Model3D* model);

        Direct3D* direct3D;
        ID3D11InputLayout* inputLayout;
        ID3D11VertexShader* vertexShader;
        ID3D11PixelShader* pixelShader;
    };
}