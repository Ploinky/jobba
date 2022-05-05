#include "mouse_input.hpp"

namespace P3D {
    short MouseInput::GetMouseX() {
        return x;
    }

    short MouseInput::GetMouseY() {
        return y;
    }

    void MouseInput::SetMousePos(short x, short y) {
        this->x = x;
        this->y = y;
    }
}