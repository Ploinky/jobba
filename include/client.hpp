// Main game application

namespace P3D {
    // Forward declarations
    class Model3D;
    class Direct3D;
    class Renderer;
    class Window;
    
    class Client {
    public:
        virtual void Run();

    private:
        bool isRunning;
        Direct3D* direct3D;
        Window* window;
        Renderer* renderer;

        // Clears the screen and/or prepares rendering
        void BeginRender();
        // Render the specified 3D model to the screen
        void Render(Model3D* model);
        // Final rendering operations and presents the rendered image to the screen
        void FinishRender();
    };
}