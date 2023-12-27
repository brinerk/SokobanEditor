build:
	clang -lraylib main.c -o Editor

debug:
	gcc -g main.c -lraylib -o debug

test:
	./Editor
