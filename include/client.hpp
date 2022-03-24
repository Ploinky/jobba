// Main game application

namespace P3D {
    // Forward declarations
    class Model3D;
    
    class Client {
    public:
        virtual void Run();

    private:
        bool isRunning;
    };
}