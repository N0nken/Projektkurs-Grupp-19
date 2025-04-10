SRCDIR=.\source
CC=gcc
BINDIR=.\bin

helloSDL: $(BINDIR)\main.o $(BINDIR)\movement.o
	$(CC) -o main $(BINDIR)\main.o $(BINDIR)\attacks.o $(BINDIR)\player.o $(BINDIR)\collision.o $(BINDIR)\vector2.o $(BINDIR)\mathex.o $(BINDIR)\movement.o -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

$(BINDIR)\main.o: $(SRCDIR)\main.c $(BINDIR)\attacks.o
	$(CC) -c -g -IC:\msys64\mingw64\include\SDL2 $(SRCDIR)\main.c -o $(BINDIR)\main.o

$(BINDIR)\attacks.o: $(SRCDIR)\attacks.c $(BINDIR)\player.o
	$(CC) -c -g $(SRCDIR)\attacks.c -o $(BINDIR)\attacks.o

$(BINDIR)\movement.o: $(SRCDIR)\movement.c
	$(CC) -c -g $(SRCDIR)\movement.c -o $(BINDIR)\movement.o


$(BINDIR)\player.o: $(SRCDIR)\player.c $(BINDIR)\collision.o
	$(CC) -c -g $(SRCDIR)\player.c -o $(BINDIR)\player.o

$(BINDIR)\collision.o: $(SRCDIR)\collision.c $(BINDIR)\vector2.o
	$(CC) -c -g $(SRCDIR)\collision.c -o $(BINDIR)\collision.o

$(BINDIR)\vector2.o: $(SRCDIR)\vector2.c $(BINDIR)\mathex.o
	$(CC) -c -g -lm $(SRCDIR)\vector2.c -o $(BINDIR)\vector2.o

$(BINDIR)\mathex.o: $(SRCDIR)\mathex.c
	$(CC) -c -g $(SRCDIR)\mathex.c -o $(BINDIR)\mathex.o

clean:
	del *.exe
	del *.o