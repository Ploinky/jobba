FOLDERS = build obj
EXE = build/jobba.exe

all: $(FOLDERS) $(EXE)

$(FOLDERS):
	if not exist .\obj\ mkdir obj
	if not exist .\build\ mkdir build

$(EXE): obj/main.obj obj/map.obj obj/r_main.obj obj/r_smap.obj obj/r_dmap.obj
	clang-cl $^ -o $@

obj/%.obj: code/src/%.c code/include/%.h
	clang-cl $< -c -I code/include -o $@

clean:
	if exist .\obj\ rmdir /S /Q .\obj
	if exist  .\build\ rmdir /S /Q .\build

run:
	$(EXE)