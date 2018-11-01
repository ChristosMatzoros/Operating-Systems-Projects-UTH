all: find_roots_lib

find_roots_lib: roots.o libroots.a
	gcc find_roots_lib.c -o find_roots_lib -lroots -L.

roots.o: roots.c roots.h
	gcc -Wall -c roots.c -o roots.o

libroots.a: roots.o
	ar rcs libroots.a roots.o

clean:
	rm -f find_roots_lib *.o *.a
