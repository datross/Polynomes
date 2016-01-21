all: main polynomial tests
	gcc -Wall main.o polynomial.o -o polynome -lm
	gcc -Wall main_test.o polynomial.o tests.o -o test -lm
    
main: polynomial tests
	gcc -Wall main.c -c
	gcc -Wall main_test.c -c
    
tests: polynomial
	gcc -Wall tests.c -c

polynomial:
	gcc -Wall polynomial.c -c
