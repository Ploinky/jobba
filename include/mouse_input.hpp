#pragma once

namespace P3D {
    class MouseInput {
        public:
            void SetMousePos(short x, short y);
            short GetMouseX();
            short GetMouseY();

        private:
            short x, y = 0;
    };
}