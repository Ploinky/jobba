#include <Windows.h>

namespace P3D {
    // Forward declarations to avoid too many headers
    class Direct3D;
    class Model3D;

    class Window {
    public:
        Window();
        ~Window();
        
        // ----- General window methods -----
        // Show window on screen
        void Show();
        // Handle window events once per frame if applicable.
        // Depending on platform implementation, this could trigger window resized, key presses, mouse movement and much more...
        void HandleEvents();
        // Will return true whenever the window wants to close or is already closed
        bool ShouldClose();
        // Clears the screen and/or prepares rendering
        void BeginRender();
        // Render the specified 3D model to the screen
        void Render(Model3D* model);
        // Final rendering operations and presents the rendered image to the screen
        void FinishRender();

        // ----- Windows specific window methods -----
        // Set value of 'shouldClose' to true. Can not be reversed -> window WILL close and P3D WILL quit!
        void SetShouldClose();

    private:
        Direct3D *direct3D;
        HWND windowHandle;
        bool shouldClose;
    };
}