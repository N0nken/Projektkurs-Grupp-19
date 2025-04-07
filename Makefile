helloSDL: main.o
	gcc -o main main.o .\source\vector2.o -lmingw32 -lSDL2main -lSDL2

main.o: .\source\main.c .\source\vector2.o
	gcc -c -g -IC:\msys64\mingw64\include\SDL2 .\source\main.c

vector2.o: .\source\vector2.c
	gcc -c -g -lm .\source\main.c

clean:
	rm *.exe
	rm *.o