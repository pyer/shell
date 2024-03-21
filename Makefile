CC = gcc
CFLAGS = -m64 -I./src

default: main.o lexer.o parser.o syntax.o execute.o signals.o
	$(CC) $(CFLAGS) main.o lexer.o parser.o syntax.o execute.o signals.o -o shell
#	@rm *.o

main.o: src/main.c src/lexer.h src/parser.h src/execute.h src/signals.h
	$(CC) $(CFLAGS) -c src/main.c

lexer.o: src/lexer.c src/lexer.h
	$(CC) $(CFLAGS) -c src/lexer.c 

parser.o: src/parser.c src/parser.h
	$(CC) $(CFLAGS) -c src/parser.c

syntax.o: src/syntax.c src/syntax.h
	$(CC) $(CFLAGS) -c src/syntax.c 

execute.o: src/execute.c src/execute.h src/syntax.h src/signals.h
	$(CC) $(CFLAGS) -c src/execute.c

signals.o: src/signals.c src/signals.h
	$(CC) $(CFLAGS) -c src/signals.c

clean:
	rm -f *.o
	find ./ -name "*~" -delete

