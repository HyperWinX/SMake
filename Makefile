all:
	gcc -O2 -march=native main.c -o smake

debug:
	gcc -g main.c -o smake
