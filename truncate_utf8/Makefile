all:
	gcc trunc.c -o trunc && ./trunc
build:
	gcc trunc.c -o trunc
debug:
	gcc -g trunc.c -o trunc && gdb ./trunc
test: build
	diff --color=always <(./trunc) <(cat expected)
