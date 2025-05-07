# Makefile para o projeto sans_battle (compatível com WSL)

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -I./raylib/src
LDFLAGS = -L./raylib/src -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Detectar plataforma (WSL vs Windows nativo)
PLATFORM = $(shell uname -s)
ifeq ($(PLATFORM),Linux)
    # Configurações para WSL/Linux
    RM = rm -f
    EXE_EXT = 
else
    # Configurações para Windows nativo
    RM = del /Q
    EXE_EXT = .exe
    LDFLAGS = -L./raylib/src -lraylib -lopengl32 -lgdi32 -lwinmm
endif

# Nome do executável
TARGET = sans_battle$(EXE_EXT)

# Arquivos fonte
SRC = sans_battle.c
OBJ = $(SRC:.c=.o)

# Regras
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c $< $(CFLAGS)

# Compilar raylib (se necessário)
raylib:
	cd raylib/src && make PLATFORM=PLATFORM_DESKTOP

# Limpar arquivos gerados
clean:
	$(RM) $(OBJ) $(TARGET)

# Limpar tudo, incluindo raylib
cleanall: clean
	cd raylib/src && make clean

# Executar o jogo
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean cleanall run raylib 