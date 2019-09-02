CC = gcc
SRC = main.c
LIB = -lm

test:
	$(CC) $(SRC) -D TEST -o solver $(LIB)
	
debug:
	$(CC) $(SRC) -o solver $(LIB)
	
all:
	$(CC) $(SRC) -D NDEBUG -o solver $(LIB)
	
run:
	./solver
