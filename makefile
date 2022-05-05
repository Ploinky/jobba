SRC_DIR := src
OBJ_DIR := build/obj
PS_SRC := shaders/pixel
VS_SRC := shaders/vertex
PS_VS_DEST := build/shaders
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
PS_IN := $(wildcard $(PS_SRC)/*.hlsl)
PS_OUT := $(patsubst $(PS_SRC)/%.hlsl,$(PS_VS_DEST)/%.cso,$(PS_IN))
VS_IN := $(wildcard $(VS_SRC)/*.hlsl)
VS_OUT := $(patsubst $(VS_SRC)/%.hlsl,$(PS_VS_DEST)/%.cso,$(VS_IN))
EXE_FILE := build/bin/jobba.exe
FOLDERS = build build/bin build/shaders build/obj
all: $(FOLDERS) $(PS_OUT) $(VS_OUT) $(EXE_FILE)

$(FOLDERS):
	if not exist .\build\bin mkdir .\build\bin
	if not exist .\build\shaders mkdir .\build\shaders
	if not exist .\build\obj mkdir .\build\obj

$(EXE_FILE): $(OBJ_FILES)
	clang++ -Wall -std=c++17 $^ -I include -o $@ -l d3d11

build/obj/%.o: src/%.cpp include/%.hpp
	clang++ -Wall -std=c++17 $< -c -I include -o $@

$(PS_OUT): $(PS_IN)
	fxc -T ps_5_0 /Fo $@ $<

$(VS_OUT): $(VS_IN)
	fxc -T vs_5_0 /Fo $@ $<

run:
	.\build\bin\jobba.exe

clean:
	if exist .\build rmdir /S /Q build