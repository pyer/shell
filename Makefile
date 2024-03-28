CC = gcc
CFLAGS = -m64 -I./src

default: main.o lexer.o parser.o node.o execute.o builtins.o signals.o
	$(CC) $(CFLAGS) main.o lexer.o parser.o node.o execute.o builtins.o signals.o -o shell
#	@rm *.o

main.o: src/main.c src/lexer.h src/parser.h src/node.h src/execute.h src/signals.h
	$(CC) $(CFLAGS) -c src/main.c

lexer.o: src/lexer.c src/lexer.h
	$(CC) $(CFLAGS) -c src/lexer.c 

parser.o: src/parser.c src/parser.h src/node.h
	$(CC) $(CFLAGS) -c src/parser.c

node.o: src/node.c src/node.h
	$(CC) $(CFLAGS) -c src/node.c 

execute.o: src/execute.c src/execute.h src/node.h src/builtins.h src/signals.h
	$(CC) $(CFLAGS) -c src/execute.c

builtins.o: src/builtins.c src/builtins.h src/version.h
	$(CC) $(CFLAGS) -c src/builtins.c

signals.o: src/signals.c src/signals.h
	$(CC) $(CFLAGS) -c src/signals.c

clean:
	rm -f *.o
	find ./ -name "*~" -delete

