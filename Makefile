osh.o : osh.c
	gcc -o osh osh.c -Wall -Werror

clean:
	rm -f osh