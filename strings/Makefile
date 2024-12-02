all:
	nasm -g -felf64 hamming.s
	gcc -g -no-pie hamming.o test.c

clean:
	rm a.out hamming.o