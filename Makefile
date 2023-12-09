all:
	gcc -O2 -march=native main.c -o smake

debug:
	gcc -O0 -g main.c -o smake
	./smake
