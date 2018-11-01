all: write_test read_test read_write_test

write_test : write_test.c
	gcc -Wall -g write_test.c -o write_test

read_test : read_test.c
	gcc -Wall -g read_test.c -o read_test

read_write_test : read_write_test.c
	gcc -Wall -g read_write_test.c -o read_write_test

clean :
	-rm -f write_test
	-rm -f read_test
	-rm -f read_write_test
