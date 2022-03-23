build:
	mkdir .\build\bin
	clang++ -Wall -std=c++17 src/main.cpp -o build/bin/jobba.exe

run:
	.\build\bin\jobba.exe

clean:
	rmdir /S /Q build