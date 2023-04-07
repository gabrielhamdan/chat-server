CC=gcc
FLAGS=-Wall -g
WINSOCKFLAG=-lwsock32
SRC=src
OBJ=obj
SRCS=$(wildcard $(SRC)/*.c)
OBJS=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))
EXE=chat-server

all: $(EXE)

$(EXE): $(OBJS)
	@echo "Compilando $(EXE)..."
	@$(CC) $(FLAGS) $(OBJS) -o $@ $(WINSOCKFLAG)
	@echo "$(EXE) compilado com sucesso!"

$(OBJ)/%.o: $(SRC)/%.c
	@ if [ ! -d "obj" ]; then echo "Criando diretório 'obj'..."; mkdir "obj"; fi

	@ $(CC) $(FLAGS) -o $@ -c $< 

clean:
	@echo "Limpando obj/*.o, *.exe"

	@ rm -f $(OBJ)/*.o
	@ rm chat-server.exe

run:
	@clear
	@./$(EXE)