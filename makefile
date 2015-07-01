limited_buffer: limited_buffer.o
	gcc -pthread -o limited_buffer limited_buffer.o -lgmp

limited_buffer.o: limited_buffer.c
	gcc -c limited_buffer.c -Wall -pedantic -ansi -g

clean:
	rm -rf *.o
	rm -rf *~
	rm limited_buffer