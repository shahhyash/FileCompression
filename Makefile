CC=gcc
CFLAGS=-Wall -Werror -fsanitize=address -std=gnu90
OUTPUTS=fileCompressor *.o *.a

all: main.c libFileCompress.a
	$(CC) $(CFLAGS) -o fileCompressor main.c libFileCompress.a

libFileCompress.a: AVL.o fileIO.o huffman.o heap.o
	ar -rs libFileCompress.a AVL.o fileIO.o huffman.o heap.o

AVL.o: data_structures/AVL.c data_structures/AVL.h
	$(CC) $(CFLAGS) -c data_structures/AVL.c

heap.o: data_structures/heap.c data_structures/heap.h
	$(CC) $(CFLAGS) -c data_structures/heap.c

fileIO.o: fileIO.c fileIO.h
	$(CC) $(CFLAGS) -c fileIO.c

huffman.o: huffman.c huffman.h
	$(CC) $(CFLAGS) -c huffman.c

clean:
	rm $(OUTPUTS)
