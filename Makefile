CC = g++
SRCS = main.cpp lexer.cpp ASTNode.cpp
PROG = compiler
CFLAGS = -std=c++11

all:
	$(CC) -o $(PROG) $(SRCS) $(CFLAGS)

clean:
	rm -f $(PROG)
