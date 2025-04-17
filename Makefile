# ----------------------------------------------------------
# Projektkataloger
# ----------------------------------------------------------
SRCDIR := source
BINDIR := bin

# ----------------------------------------------------------
# Plattformdetektion + verktyg / flaggor
# ----------------------------------------------------------
ifeq ($(OS),Windows_NT)                    # MSYS / Git‑Bash / mingw32‑make
    PLATFORM   := WINDOWS
    CC         := gcc
    CFLAGS_SDL := -IC:/msys64/mingw64/include/SDL2
    LIBS_SDL   := -lmingw32 -lSDL2main -lSDL2 -lSDL2_image
    MKDIR      := @if not exist "$(BINDIR)" mkdir "$(BINDIR)"
    RM         := rm -f
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Darwin)               # macOS (Homebrew)
        PLATFORM   := MAC
        CC         := clang                
        CFLAGS_SDL := $(shell sdl2-config --cflags) $(shell pkg-config --cflags SDL2_image)
        LIBS_SDL   := $(shell sdl2-config --libs)   $(shell pkg-config --libs SDL2_image)
        MKDIR      := @mkdir -p $(BINDIR)
        RM         := rm -f
    else
        $(error Unsupported platform: $(UNAME_S))
    endif
endif

CFLAGS  := -g $(CFLAGS_SDL)
LDFLAGS := $(LIBS_SDL)
LDLIBS  := -lm

# ----------------------------------------------------------
# Målet
# ----------------------------------------------------------
EXECUTABLE := main$(if $(filter $(PLATFORM),WINDOWS),.exe,)

# ----------------------------------------------------------
# Byggregler
# ----------------------------------------------------------

helloSDL: $(BINDIR)/main.o $(BINDIR)/movement.o $(BINDIR)/input_logger.o
	$(CC) -o $(EXECUTABLE) $(BINDIR)/main.o \
	      $(BINDIR)/attacks.o $(BINDIR)/player.o $(BINDIR)/collision.o \
	      $(BINDIR)/vector2.o $(BINDIR)/mathex.o \
	      $(BINDIR)/movement.o $(BINDIR)/input_logger.o $(LDFLAGS)

# ---------------- main.o ----------------
$(BINDIR)/main.o: $(SRCDIR)/main.c $(BINDIR)/attacks.o
	$(MKDIR)
	$(CC) $(CFLAGS) -c $(SRCDIR)/main.c -o $@

# ---------------- attacks.o -------------
$(BINDIR)/attacks.o: $(SRCDIR)/attacks.c $(BINDIR)/player.o $(BINDIR)/input_logger.o
	$(MKDIR)
	$(CC) $(CFLAGS) -c $(SRCDIR)/attacks.c -o $@

# ---------------- movement.o ------------
$(BINDIR)/movement.o: $(SRCDIR)/movement.c
	$(MKDIR)
	$(CC) $(CFLAGS) -c $(SRCDIR)/movement.c -o $@

# ---------------- player.o --------------
$(BINDIR)/player.o: $(SRCDIR)/player.c $(BINDIR)/collision.o $(BINDIR)/input_logger.o
	$(MKDIR)
	$(CC) $(CFLAGS) -c $(SRCDIR)/player.c -o $@

# ---------------- collision.o -----------
$(BINDIR)/collision.o: $(SRCDIR)/collision.c $(BINDIR)/vector2.o
	$(MKDIR)
	$(CC) $(CFLAGS) -c $(SRCDIR)/collision.c -o $@

# ---------------- vector2.o -------------
$(BINDIR)/vector2.o: $(SRCDIR)/vector2.c $(BINDIR)/mathex.o
	$(MKDIR)
	$(CC) $(CFLAGS) -c $(SRCDIR)/vector2.c -o $@

# ---------------- mathex.o --------------
$(BINDIR)/mathex.o: $(SRCDIR)/mathex.c
	$(MKDIR)
	$(CC) $(CFLAGS) -c $(SRCDIR)/mathex.c -o $@

# ---------------- input_logger.o --------
$(BINDIR)/input_logger.o: $(SRCDIR)/input_logger.c
	$(MKDIR)
	$(CC) $(CFLAGS) -c $(SRCDIR)/input_logger.c -o $@

# ----------------------------------------------------------
#  Städning
# ----------------------------------------------------------
.PHONY: clean
clean:
	-$(RM) $(BINDIR)/*.o $(EXECUTABLE)