all: main

hamming.o:
	nasm -felf64 hamming.s

b64.o:
	gcc -c -msse3 b64.c

main: hamming.o b64.o
	gcc -msse3 hamming.o b64.o main.c

clean:
	rm -f a.out *.o