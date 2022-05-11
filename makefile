FOLDERS = build obj


all: $(FOLDERS) build/wnw.exe

$(FOLDERS):
	if not exist .\obj\ mkdir obj
	if not exist .\build\ mkdir build

build/wnw.exe: obj/main.obj
	clang-cl -Wall $^ -o $@

obj/%.obj: code/src/%.c code/include/%.h
	clang-cl -Wall $< -c -I . -o $@

clean:
	if exist .\obj\ rmdir /S /Q .\obj
	if exist  .\build\ rmdir /S /Q .\build

run:
	.\build\wnw.exe