CC=gcc
FLAGS=-Wall -g
SRC=src
OBJ=obj
SRCS=$(wildcard $(SRC)/*.c)
OBJS=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))
EXE=chat-client

all: $(EXE)

$(EXE): $(OBJS)
	@echo "Compilando $(EXE)..."
	@$(CC) $(FLAGS) $(OBJS) -o $@
	@echo "$(EXE) compilado com sucesso!"

$(OBJ)/%.o: $(SRC)/%.c
	@ if [ ! -d "obj" ]; then echo "Criando diretório 'obj'..."; mkdir "obj"; fi

	@ $(CC) $(FLAGS) -o $@ -c $< 

.PHONY: clean
clean:
	@clear
	@echo "Limpando obj/*.o, chat-client"

	@ -rm -f $(OBJ)/*.o
	@ -rm ./chat-client

run:
	@clear
	@./$(EXE)