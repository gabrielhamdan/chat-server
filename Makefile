CC=gcc
FLAGS=-Wall -g
SRC=src
OBJ=obj
SRCS=$(wildcard $(SRC)/*.c)
OBJS=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))
EXE=chat-server

all: $(EXE)

$(EXE): $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $@

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(FLAGS) -o $@ -c $< 

clean:
	rm -f $(OBJ)/*.o
	rm chat-server.exe

run:
	./$(EXE)