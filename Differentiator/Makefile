CC = g++
FLAGS = -fsanitize=address -fsanitize=undefined -g -std=c++17
SRC = src/*
INC = inc

all:
	$(CC) $(FLAGS) $(SRC) -I$(INC) -o dif
	./dif
	dot -Tpng dif.dot -o dif.png
	xdg-open dif.png
	
