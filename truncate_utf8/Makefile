all:
	gcc trunc.c -o trunc && ./trunc
debug:
	gcc -g trunc.c -o trunc && gdb ./trunc
test:
	diff --color=always <(./trunc) <(cat expected)
