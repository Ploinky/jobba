#include <Windows.h>
#include <functional>
#include <vector>

namespace P3D {
    // Forward declarations to avoid too many headers
    class Direct3D;
    class Model3D;

    class Window {
    public:
        Window();
        ~Window();

        HWND GetWindowHandle();
        
        // Show window on screen
        void Show();
        // Handle window events once per frame if applicable.
        // Depending on platform implementation, this could trigger window resized, key presses, mouse movement and much more...
        void HandleEvents();
        // Will return true whenever the window wants to close or is already closed
        bool ShouldClose();
        // Set value of 'shouldClose' to true. Can not be reversed -> window WILL close and P3D WILL quit!
        void SetShouldClose();

        void Resized(int width, int height);

        std::function<void()> callback;

        int width;
        int height;

    private:
        HWND windowHandle;
        bool shouldClose;

        void Notify();
    };
}