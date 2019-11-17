CC = g++
FLAGS = -fsanitize=address -fsanitize=undefined -g
SRC = src/*
INC = inc

all:
	$(CC) $(FLAGS) $(SRC) -I$(INC) -o dif
