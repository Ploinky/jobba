build:
	if not exist .\build\bin mkdir .\build\bin
	clang++ -Wall -std=c++17 src/*.cpp -I include -o build/bin/jobba.exe -l d3d11

buildNoWarn:
	if not exist .\build\bin mkdir .\build\bin
	clang++ -std=c++17 src/*.cpp -I include -o build/bin/jobba.exe -l d3d11

run:
	.\build\bin\jobba.exe

clean:
	rmdir /S /Q build