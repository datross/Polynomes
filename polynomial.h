/* polynomial.h */

#ifndef POLYNOMIAL_MODULE
#define POLYNOMIAL_MODULE

#include <stdio.h>

#define BUFFER_READING 10000

/** --- Polynomial structures --- **/


typedef struct Monomial {
    double coefficient;
    long degree;
    struct Monomial * next;
} Monomial;

typedef Monomial List;

typedef struct {
    List * monomials;
    long degree;
} Polynomial;


/** --- Error codes --- **/


typedef enum error_code {
    NONE,            // no error
    MEM_ALLOC,       // memory allocation failed
    NULL_PTR,        // a pointer in the arguments is null and should not be
    POLY_FORMAT,     // specific to reading a polynomial : string has not the right polynomial format
    DIVISION_BY_ZERO,// speaks by itself. Used by the division function.
    READING_ERROR    // if fgets can't read the given stream.
} error_code;

/* Prints a message corresponding to the error, in the given stream.
 * If err is a NULL pointer, or if it is 'none', it prints nothing.
 * Last argument enables the user to print additional details. */
void printError(FILE * stream, error_code * err, char * details);


/** --- Functions --- **/


/* returns a pointer of a new monomial pointer, intialized if the given 
 * coefficient and degree, returns NULL if an error occured, and 
 * places the code in 'err'. */
Monomial * createMonomial(double coefficient, long degree, error_code * err);

/* returns a new null polynomial pointer.
 * Returns NULL if an error occured,
 * and places the code in 'err'. */
Polynomial * createNullPolynomial(error_code * err);

/* free a List. */
void freeList(List * list);

/* free a Polynomial. */
void freePolynomial(Polynomial * poly);

/* Returns a copy of 'src'.
 * It does not check validity if the list.
 * Places eventual error in 'err'. */
List * copyList(List * src, error_code * err);

/* Returns a copy of 'src'.
 * It does not check validity of the polynomial.
 * Places eventual error in 'err'. */
Polynomial * copyPolynomial(Polynomial * src, error_code * err);

/* Prints the given polynomial in the 'stream'.
 * Places eventual error in 'err'. */
void printPolynomial(FILE * stream, Polynomial * poly, error_code * err);

/* Reads polynomial from the given 'stream'.
 * Returns NULL if an error occured.
 * Places eventual error in 'err'. */
Polynomial * readPolynomial(FILE * stream, error_code * err);

/* returns a polynomial pointer, from the given string. Places eventual errors code 
 * in 'err', and if so, returns eventually NULL (it tries to read polynomial even 
 * with errors).
 * Reading format : "%lf %ld %lf %ld ..." */
Polynomial * parsePolynomial(char * poly_str, error_code * err);
 
/* adds polynomials, stores result in Polynomial type argument.
 * Keeps the 'by degree' order. Both 'poly' and 'list_to_add' must be sorted by degree order.
 * Places eventual error in 'err'. */
void addPolynomials(Polynomial * poly, List * list_to_add, error_code * err);

/* Differentiates polynomial about 'X', and keeps the polynomial ordered.
 * Places eventual error in 'err'. */
void differentiatePolynomial(Polynomial * poly, error_code * err);

/* Multiply 'poly' with the given polynomial factor ('factor' isn't modified).
 * It keeps ordered 'poly'.
 * Places eventual error in 'err'. */
void multiplyPolynomials(Polynomial * poly, List * factor, error_code * err);

/* Elevates 'poly' to power 'n'.
 * This function uses the exponentation by squaring method.
 * Eventual error is placed in 'err'. */
void powerPolynomial(Polynomial * poly, unsigned int n, error_code * err);
 
/* Evaluates 'poly' value at 'x', and returns this value.
 * This function uses a basic algorithm (not Horner).
 * Eventual error is places in 'err'. */
double evaluatePolynomial(Polynomial * poly, double x, error_code * err);

/* Evaluates 'poly' value at 'x', and returns this value.
 * This function uses Horner algorithm.
 * Eventual error is places in 'err'. */
double evaluatePolynomialHorner(Polynomial * poly, double x, error_code * err);

/* Divides dividend by divisor (they aren't modified).
 * If 'quotient' and 'remainder' aren't respectivly null, the quotient and remainder of
 * the division are respectivly placed in the corresponding argument.
 * Places eventual error in 'err'. */
void dividePolynomials(List * dividend, List * divisor, Polynomial * quotient, Polynomial * remainder, error_code * err);

/* Returns a pointer toward gcd of a and b (a and b aren't modified). 
 * It returns a monic polynomial.
 * Conventions : gcd(0,P) = gcd(P,0) = P et gcd(0,0) = 0
 * In error case, it returns NULL and places error_code in 'err'. */
Polynomial * gcdPolynomials(List * a, List * b, error_code * err);

#endif //POLYNOMIAL_MODULE
