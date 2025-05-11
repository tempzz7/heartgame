# Makefile para o projeto heart_battle (Linux, usando raylib local)

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -I./raylib/src
LDFLAGS = -L./raylib/src -lraylib -lcurl -lm -lpthread -ldl -lrt -lX11

# Nome do executável
TARGET = heartgame

# Arquivos fonte
SRC = main.c game.c player.c attack.c hud.c utils.c
OBJ = $(SRC:.c=.o)

# Regras
all: rayliblib $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c $< $(CFLAGS)

# Compilar raylib (se necessário)
rayliblib:
	$(MAKE) -C raylib/src PLATFORM=PLATFORM_DESKTOP

# Limpar arquivos gerados
clean:
	rm -f $(OBJ) $(TARGET)

# Limpar tudo, incluindo raylib
cleanall: clean
	$(MAKE) -C raylib/src clean

# Executar o jogo
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean cleanall run rayliblib