CC=gcc
FLAGS=-Wall -g
SRC=src
OBJ=obj
SRCS=$(wildcard $(SRC)/*.c)
OBJS=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))
EXE=chat-server

all: $(EXE)

$(EXE): $(OBJS)
	@echo "Compilando..."
	$(CC) $(FLAGS) $(OBJS) -o $@
	@echo "Pronto!"

$(OBJ)/%.o: $(SRC)/%.c
	@ if [ ! -d "obj" ]; then echo "Criando diretório 'obj'..."; mkdir "obj"; fi

	@ $(CC) $(FLAGS) -o $@ -c $< 

clean:
	@echo "Limpando obj/*.o, *.exe"

	@ rm -f $(OBJ)/*.o
	@ rm chat-server.exe

run:
	./$(EXE)