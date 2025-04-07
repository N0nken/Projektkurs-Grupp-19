helloSDL: main.o
	gcc -o main main.o .\source\vector2.o -lmingw32 -lSDL2main -lSDL2

main.o: .\source\main.c .\source\vector2.o .\source\player_character.o .\source\collision.o
	gcc -c -g -IC:\msys64\mingw64\include\SDL2 .\source\main.c

player_character.o: .\source\player_character.c .\source\vector2.o .\source\collision.o

collision.o: .\source\collision.c .\source\vector2.o
	gcc -c -g -lm .\source\collision.c

vector2.o: .\source\vector2.c
	gcc -c -g -lm .\source\vector2.c

mathex.o: .\source\mathex.c
	gcc -c -g .\source\mathex.c

clean:
	rm *.exe
	rm *.o