FOLDERS := build obj
EXE := build/jobba.exe
OBJ_DIR := obj
SRC_DIR := code/src
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.obj,$(SRC_FILES))

all: $(FOLDERS) $(EXE)

$(FOLDERS):
	if not exist .\obj\ mkdir obj
	if not exist .\build\ mkdir build

$(EXE): $(OBJ_FILES)
	clang -g $^ -o $@

obj/%.obj: code/src/%.c code/include/%.h
	clang $< -c -I code/include -o $@

clean:
	if exist .\obj\ rmdir /S /Q .\obj
	if exist  .\build\ rmdir /S /Q .\build

run:
	$(EXE)