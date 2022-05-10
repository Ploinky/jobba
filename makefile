build/wnw.exe: obj/main.obj
	clang-cl -Wall $^ -o $@

obj/main.obj: main.c main.h
	clang-cl -Wall $< -c -I .

clean:
	del obj/main.obj
	del build/wnw.exe