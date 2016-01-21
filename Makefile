all: main polynomial
	gcc -Wall main.o polynomial.o tests.o -o test -lm
    
main: polynomial tests
	gcc -Wall main.c -c
    
tests: polynomial
	gcc -Wall tests.c -c

polynomial:
	gcc -Wall polynomial.c -c
