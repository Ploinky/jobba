// Main game application

namespace P3D {
    // Forward declarations
    class Model3D;
    class Direct3D;
    class Renderer;
    class Window;
    class KeyboardInput;
    class MouseInput;
    
    class Client {
    public:
        virtual void Run();

    private:
        // Indicates whether the Client is running and should continue running
        bool isRunning;
        // Access to DirectX 11 resources
        Direct3D* direct3D;
        // Access to native window 
        Window* window;
        // Access to DirectX 11 rendering pipeline
        Renderer* renderer;

        long long lastFrame;

        KeyboardInput* keyboardInput;
        MouseInput* mouseInput;

        // Clears the screen and prepares rendering
        void BeginRender();
        // Render the specified 3D model to the screen
        void Render(Model3D* model);
        // Final rendering operations and presents the rendered image to the screen
        void FinishRender();

        void HandlePlayerInput(Model3D* model, float dt);

        long long GetSystemTime();
    };
}