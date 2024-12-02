all: main

formula1.o:
	gcc -g -msse3 -c formula1.c -lm

formula2.o:
	# nasm -g -felf64 formula2.s
	gcc -g -msse3 -c formula2.c

main: formula1.o formula2.o
	gcc -g formula1.o formula2.o main.c -lm

clean:
	rm -rfd *.o a.out