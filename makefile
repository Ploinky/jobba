build:
	if not exist .\build\bin mkdir .\build\bin
	if not exist .\build\shaders mkdir .\build\shaders
	clang++ -Wall -std=c++17 src/*.cpp -I include -o build/bin/jobba.exe -l d3d11
	fxc -Fo .\build\shaders\vertex.cso .\shaders\vertex.hlsl

buildNoWarn:
	if not exist .\build\bin mkdir .\build\bin
	if not exist .\build\shaders mkdir .\build\shaders
	clang++ -std=c++17 src/*.cpp -I include -o build/bin/jobba.exe -l d3d11
	fxc -Fo .\build\shaders\vertex.cso .\shaders\vertex.hlsl

run:
	.\build\bin\jobba.exe

clean:
	rmdir /S /Q build