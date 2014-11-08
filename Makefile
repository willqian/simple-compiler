CC = g++
SRCS = lexer.cpp ASTNode.cpp AST.cpp parser.cpp
PROG = compiler
CAL = calculator
CFLAGS = -std=c++11

all:
	$(CC) -o $(PROG) main.cpp $(SRCS) $(CFLAGS)
	$(CC) -o $(CAL) cal.cpp $(SRCS) $(CFLAGS)

cal:
	$(CC) -o $(CAL) cal.cpp $(SRCS) $(CFLAGS)

clean:
	rm -f $(PROG) $(CAL)
