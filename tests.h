/* tests.h */

#ifndef TESTS
#define TESTS

#include <stdio.h>
#include "polynomial.h"

/** --- Useful functions --- **/

/* Returns 1 if p1=p2, even if there are NULL. Returns 0 else. */
int comparePolynomials(Polynomial * p1, Polynomial * p2);

/* Returns 1 if 'poly' is valid, 0 if not or if NULL. */
int checkPolynomial(Polynomial * poly);

/** --- Unit test functions --- **/

/* All these functions return 1 if the test succeed, 0 else */

int test_all();
int test_createNullPolynomial();
int test_copyPolynomial();
int test_addPolynomials();
int test_parsePolynomial();
int test_differentiatePolynomial();
int test_multiplyPolynomials();
int test_powerPolynomial();
int test_evaluatePolynomial();
int test_evaluatePolynomialHorner();
int test_dividePolynomials();
int test_gcdPolynomials();

#endif // TESTS
