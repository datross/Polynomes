/* main.c */

#include <stdio.h>
#include "polynomial.h"

int main() {
    /*
     * This program stores in a file a polynomial entered by the user,
     * close the file, reopens it and read the polynomial in the stdout.
     */
     
    error_code err = NONE;    
    
    FILE * file = fopen("poly.txt", "w");
    
    Polynomial * p = readPolynomial(stdin, &err);
    if(!p) {
        printError(stdout, &err, NULL);
        return 0;
    }
    printPolynomial(file, p, &err);
    if(err != NONE) {
        printError(stdout, &err, NULL);
        freePolynomial(p);
        return 0;
    }
    
    freePolynomial(p);
    fclose(file);
    
    file = fopen("poly.txt", "r");
    
    p = readPolynomial(file, &err);
    if(!p) {
        printError(stdout, &err, NULL);
        return 0;
    }
    printPolynomial(stdout, p, &err);
    if(err != NONE) {
        printError(stdout, &err, NULL);
        freePolynomial(p);
        return 0;
    }
    
    freePolynomial(p);
    fclose(file);
    
    return 0;
}
