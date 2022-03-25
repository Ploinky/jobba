#include "d3d11.h"
#include <stdint.h>

namespace P3D {
    class Direct3D;
    class Vertex;

    class Model3D {
        public:
            bool Initialize(Direct3D* direct3D);
            bool IsInitialized();

            Vertex* vertices;
            uint32_t vertexCount;
            unsigned int* indices;
            uint32_t indexCount;
            ID3D11Buffer* vertexBuffer;
            ID3D11Buffer* indexBuffer;

        private:
            bool initialized = false;
    };
}