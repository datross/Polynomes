/* tests.c */

#include "tests.h"

int comparePolynomials(Polynomial * p1, Polynomial * p2) {
    if(!p1 || !p2) {
        if(p1 != p2)
            return 0;
        return 1;
    }
    
    if(p1->degree != p2->degree)
        return 0;

    Monomial *m1 = p1->monomials, *m2 = p2->monomials;
    for(; m1 && m2; m1 = m1->next, m2 = m2->next) {
        if(m1->coefficient != m2->coefficient 
            || m1->degree != m2->degree)
            return 0;
    }
    
    if(m1 != m2)
        return 0;
    
    return 1;
}

int checkPolynomial(Polynomial * poly) {
    if(!poly)
        return 0;
    if(!(poly->monomials))
        return 0;
    if(poly->degree != poly->monomials->degree)
        return 0;
    
    Monomial * m;
    
    for(m = poly->monomials; m; m = m->next) {
        if(m->coefficient == 0 && m->degree != 0)
            return 0;
        if(m->next && m->next->degree >= m->degree)
            return 0;
        if(!(m->next) && m->degree != 0)
            return 0;
    }
    
    return 1;
}

int test_all() {
    if(!test_createNullPolynomial()) return 0;
    if(!test_copyPolynomial()) return 0;
    if(!test_addPolynomials()) return 0;
    if(!test_parsePolynomial()) return 0;
    if(!test_differentiatePolynomial()) return 0;
    if(!test_multiplyPolynomials()) return 0;
    if(!test_powerPolynomial()) return 0;
    if(!test_evaluatePolynomial()) return 0;
    if(!test_evaluatePolynomialHorner()) return 0;
    if(!test_dividePolynomials()) return 0;
    if(!test_gcdPolynomials()) return 0;
    
    return 1;
}
    

int test_createNullPolynomial() {
    printf("--- Test createNullPolynomial ---\n");
    
    error_code err = NONE;
    Polynomial * p = createNullPolynomial(&err);
    if(!p) {
        printError(stdout, &err, NULL);
        return 0;
    }
    if(!checkPolynomial(p)) {
        printf("  Polynomial not valid.\n");
        return 0;
    }
    if(p->monomials->coefficient != 0 || p->monomials->degree != 0) {
        printf("  Polynomial not null.\n");
        return 0;
    }
    
    return 1;
}

int test_copyPolynomial() {
    printf("--- Test copyPolynomial ---\n");
    
    error_code err = NONE;
    
    Monomial m1, m2;
    m1.next = &m2;
    m2.next = NULL;
    Polynomial p;
    p.monomials = &m1;
    
    Polynomial * p2 = copyPolynomial(&p, &err);
    if(!p2) {
        printError(stdout, &err, NULL);
        return 0;
    }
    
    if(!comparePolynomials(&p, p2)) {
        printf("  Not the same polynomials\n");
        return 0;
    }
    
    return 1;
}

int test_addPolynomials() {
    printf("--- Test addPolynomials ---\n");

    error_code err = NONE;
    
    Polynomial *p1 = createNullPolynomial(&err),
                *p2 = createNullPolynomial(&err);
    if(!p1 || !p2) {
        freePolynomial(p1);
        freePolynomial(p2);
        printError(stdout, &err, NULL);
        return 0;
    }
    
    /* test of 0+0 */
    addPolynomials(p1, p2->monomials, &err);
    if(err != NONE) {
        freePolynomial(p1);
        freePolynomial(p2);
        printError(stdout, &err, NULL);
        return 0;
    }
    if(!comparePolynomials(p1, p2)) {
        printf("  Result is not correct : 0+0.\n");
        freePolynomial(p1);
        freePolynomial(p2);
        return 0;
    }
    
    /* test of (X+X^2+1)+X */
    freeList(p1->monomials);
    p1->monomials = createMonomial(1, 2, &err);
    p1->monomials->next = createMonomial(1, 1, &err);
    p1->monomials->next->next = createMonomial(1, 0, &err);
    p1->degree = 2;
    
    p2->monomials->coefficient = 1;
    p2->monomials->degree = 1;
    p2->monomials->next = createMonomial(0, 0 ,&err);
    p2->degree = 1;
    
    if(err != NONE) {
        freePolynomial(p1);
        freePolynomial(p2);
        printError(stdout, &err, NULL);
        return 0;
    }
    
    addPolynomials(p2, p1->monomials, &err);
    if(err != NONE) {
        freePolynomial(p1);
        freePolynomial(p2);
        printError(stdout, &err, NULL);
        return 0;
    }
    p1->monomials->next->coefficient = 2;

    if(!comparePolynomials(p1, p2)) {
        printf("  Result is not correct : (X+X^2+1)+X.\n");
        freePolynomial(p1);
        freePolynomial(p2);
        return 0;
    }
    
    freePolynomial(p1);
    freePolynomial(p2);
    
    return 1;
}
   
int test_parsePolynomial() {
    printf("--- Test parsePolynomial ---\n");
    error_code err = NONE;
    Polynomial * p1 = createNullPolynomial(&err);
    if(!p1) {
        printError(stdout, &err, NULL);
        return 0;
    }
    
    Polynomial * p2 = parsePolynomial("", &err);
    if(!p2) {
        printError(stdout, &err, NULL);
        freePolynomial(p1);
        return 0;
    }
    
    if(!comparePolynomials(p1, p2)) {
        printf("  Wrong parsed polynomial : 0.\n");
        freePolynomial(p1);
        freePolynomial(p2);
        return 0;
    }
    
    p1->degree = 5;
    p1->monomials->coefficient = 2.8;
    p1->monomials->degree = 5;
    p1->monomials->next = createMonomial(1, 0, &err);
    if(!(p1->monomials->next)) {
        printError(stdout, &err, NULL);
        freePolynomial(p1);
        freePolynomial(p2);
        return 0;
    }
    freePolynomial(p2);
    p2 = parsePolynomial("1 0 2.8 5", &err);
    if(!p2) {
        printError(stdout, &err, NULL);
        freePolynomial(p1);
        return 0;
    }
    
    if(!comparePolynomials(p1, p2)) {
        printf("  Wrong parsed polynomial : 2.8X^5 + 1.\n");
        freePolynomial(p1);
        freePolynomial(p2);
        return 0;
    }

    freePolynomial(p1);
    freePolynomial(p2);

    return 1;
}

int test_differentiatePolynomial() {
    printf("--- Test differentiatePolynomial ---\n");
    
    error_code err = NONE;

    Polynomial * p1 = parsePolynomial("1 0", &err);
    Polynomial * p2 = parsePolynomial("0 0", &err);
    if(!p1 || !p2) {
        printError(stdout, &err, NULL);
        freePolynomial(p1);
        freePolynomial(p2);
        return 0;
    }
    
    differentiatePolynomial(p1, &err);
    if(err != NONE) {
        printError(stdout, &err, NULL);
        freePolynomial(p1);
        freePolynomial(p2);
        return 0;
    }
    
    if(!comparePolynomials(p1, p2)) {
        printf("  Wrong differentation : 1.\n");
        freePolynomial(p1);
        freePolynomial(p2);
        return 0;
    }
    
    freePolynomial(p1);
    freePolynomial(p2);
    p1 = parsePolynomial("2 5 1 3 3 2", &err);
    p2 = parsePolynomial("10 4 3 2 6 1", &err);
    if(!p1 || !p2) {
        printError(stdout, &err, NULL);
        freePolynomial(p1);
        freePolynomial(p2);
        return 0;
    }
    
    differentiatePolynomial(p1, &err);
    if(err != NONE) {
        printError(stdout, &err, NULL);
        freePolynomial(p1);
        freePolynomial(p2);
        return 0;
    }
    
    if(!comparePolynomials(p1, p2)) {
        printf("  Wrong differentation : 2X^5 + X^3 + 3X^2.\n");
        freePolynomial(p1);
        freePolynomial(p2);
        return 0;
    }

    freePolynomial(p1);
    freePolynomial(p2);
    
    return 1;
}

int test_multiplyPolynomials() {
    printf("--- Test multiplyPolynomials ---\n");
    
    error_code err = NONE;
    
    Polynomial *p1 = parsePolynomial("0 0", &err),
               *p2 = parsePolynomial("0 0", &err),
               *p3 = parsePolynomial("1 1 1 0", &err),
               *p4 = parsePolynomial("0 0", &err),
               *p5 = parsePolynomial("0 0", &err),
               *p6 = parsePolynomial("1 1 1 0", &err),
               *p7 = parsePolynomial("1 1 -1 0", &err),
               *p8 = parsePolynomial("1 1 1 0", &err),
               *p9 = parsePolynomial("1 2 -1 0", &err);
               
    if(!p1 || !p2 || !p3 || !p4 || !p5 || !p6 || !p7 || !p8 || !p9) {
        printError(stdout, &err, NULL);
        freePolynomial(p1);
        freePolynomial(p2);
        freePolynomial(p3);
        freePolynomial(p4);
        freePolynomial(p5);
        freePolynomial(p6);
        freePolynomial(p7);
        freePolynomial(p8);
        freePolynomial(p9);
        return 0;
    }
    
    multiplyPolynomials(p1, p2->monomials, &err);
    multiplyPolynomials(p3, p4->monomials, &err);
    multiplyPolynomials(p5, p6->monomials, &err);
    multiplyPolynomials(p7, p8->monomials, &err);
    
    if(err != NONE) {
        printError(stdout, &err, NULL);
        freePolynomial(p1);
        freePolynomial(p2);
        freePolynomial(p3);
        freePolynomial(p4);
        freePolynomial(p5);
        freePolynomial(p6);
        freePolynomial(p7);
        freePolynomial(p8);
        freePolynomial(p9);
        return 0;
    }
    
    if(!comparePolynomials(p1, p2)
        || !comparePolynomials(p3, p4)
        || !comparePolynomials(p5, p1)
        || !comparePolynomials(p7, p9)) {
        printf("  Wrong result for multiplication.\n");
        freePolynomial(p1);
        freePolynomial(p2);
        freePolynomial(p3);
        freePolynomial(p4);
        freePolynomial(p5);
        freePolynomial(p6);
        freePolynomial(p7);
        freePolynomial(p8);
        freePolynomial(p9);
        return 0;
    }

    freePolynomial(p1);
    freePolynomial(p2);
    freePolynomial(p3);
    freePolynomial(p4);
    freePolynomial(p5);
    freePolynomial(p6);
    freePolynomial(p7);
    freePolynomial(p8);
    freePolynomial(p9);
    
    return 1;
}

int test_powerPolynomial() {
    printf("--- Test powerPolynomial ---\n");
    
    error_code err = NONE;
    
    Polynomial *factor = parsePolynomial("1 2 1 1 1 0", &err),
                *p1 = parsePolynomial("1 0", &err),
                *p2;
    if(!factor || !p1) {
        printError(stdout, &err, NULL);
        freePolynomial(factor);
        freePolynomial(p1);
        return 0;
    }
    
    int i = 0;
    for(; i < 10; ++i) {
        p2 = copyPolynomial(factor, &err);
        if(!p2) {
            printError(stdout, &err, NULL);
            freePolynomial(factor);
            freePolynomial(p1);
            freePolynomial(p2);
            return 0;
        }
        
        powerPolynomial(p2, i, &err);
        
        if(err != NONE) {
            printError(stdout, &err, NULL);
            freePolynomial(factor);
            freePolynomial(p1);
            freePolynomial(p2);
            return 0;
        }
        
        if(!comparePolynomials(p1, p2)) {
            printf("  Wrong power : %d.\n", i);
            freePolynomial(factor);
            freePolynomial(p1);
            freePolynomial(p2);
            return 0;
        }
        
        multiplyPolynomials(p1, factor->monomials, &err);
        
        freePolynomial(p2);
        p2 = NULL;
    }
    
    freePolynomial(factor);
    freePolynomial(p1);
    freePolynomial(p2);
            
    return 1;
}

int test_evaluatePolynomial() {
    printf("--- Test evaluatePolynomial ---\n");
    
    error_code err = NONE;
    
    Polynomial *p1 = parsePolynomial("0 0", &err),
                *p2 = parsePolynomial("1 1 1 0", &err);
    if(!p1 || !p2) {
        printError(stdout, &err, NULL);
        freePolynomial(p1);
        freePolynomial(p2);
        return 0;
    }
    
    double a = evaluatePolynomial(p1, 0.2, &err);
    double b = evaluatePolynomial(p2, 0.2, &err);
    if(err != NONE) {
        printError(stdout, &err, NULL);
        freePolynomial(p1);
        freePolynomial(p2);
        return 0;
    }
    
    if(a != 0. || b != 1.2) {
        printf("  Wrong evaluation.\n");
        freePolynomial(p1);
        freePolynomial(p2);
        return 0;
    }
    
    freePolynomial(p1);
    freePolynomial(p2);
    
    return 1;
}

int test_evaluatePolynomialHorner() {
    printf("--- Test evaluatePolynomialHorner ---\n");
    
    error_code err = NONE;
    
    Polynomial *p1 = parsePolynomial("0 0", &err),
                *p2 = parsePolynomial("1 1 1 0", &err);
    if(!p1 || !p2) {
        printError(stdout, &err, NULL);
        freePolynomial(p1);
        freePolynomial(p2);
        return 0;
    }
    
    double a = evaluatePolynomialHorner(p1, 0.2, &err);
    double b = evaluatePolynomialHorner(p2, 0.2, &err);
    if(err != NONE) {
        printError(stdout, &err, NULL);
        freePolynomial(p1);
        freePolynomial(p2);
        return 0;
    }
    
    if(a != 0. || b != 1.2) {
        printf("  Wrong evaluation.\n");
        freePolynomial(p1);
        freePolynomial(p2);
        return 0;
    }
    
    freePolynomial(p1);
    freePolynomial(p2);
    
    return 1;
}

int test_dividePolynomials() {
    printf("--- Test dividePolynomials ---\n");
    
    error_code err = NONE;
    
    Polynomial *p1 = parsePolynomial("0 0", &err),
                *p2 = parsePolynomial("1 5 -1 4 -1 3 3 2 -2 1", &err),
                *p3 = parsePolynomial("1 2 -1 1 1 0", &err),
                *p4 = parsePolynomial("1 3 -2 1 1 0", &err),
                *p5 = parsePolynomial("1 1 -1 0", &err),
                *p6 = createNullPolynomial(&err),
                *p7 = createNullPolynomial(&err);
    if(!p1 || !p2 || !p3 || !p4 || !p5 || !p6 || !p7) {
        printError(stdout, &err, NULL);
        freePolynomial(p1);
        freePolynomial(p2);
        freePolynomial(p3);
        freePolynomial(p4);
        freePolynomial(p5);
        freePolynomial(p6);
        freePolynomial(p7);
        return 0;
    }
    
    dividePolynomials(p1->monomials, p2->monomials, p6, p7, &err);
    if(err != NONE) {
        printError(stdout, &err, NULL);
        freePolynomial(p1);
        freePolynomial(p2);
        freePolynomial(p3);
        freePolynomial(p4);
        freePolynomial(p5);
        freePolynomial(p6);
        freePolynomial(p7);
        return 0;
    }
    
    if(!comparePolynomials(p1, p6) || !comparePolynomials(p1, p7)) {
        printf("  Wrong division : 0 / A.\n");
        freePolynomial(p1);
        freePolynomial(p2);
        freePolynomial(p3);
        freePolynomial(p4);
        freePolynomial(p5);
        freePolynomial(p6);
        freePolynomial(p7);
        return 0;
    }
    
    dividePolynomials(p2->monomials, p3->monomials, p6, p7, &err);
    if(err != NONE) {
        printError(stdout, &err, NULL);
        freePolynomial(p1);
        freePolynomial(p2);
        freePolynomial(p3);
        freePolynomial(p4);
        freePolynomial(p5);
        freePolynomial(p6);
        freePolynomial(p7);
        return 0;
    }
    
    if(!comparePolynomials(p4, p6) || !comparePolynomials(p5, p7)) {
        printf("  Wrong division : A / B.\n");
        freePolynomial(p1);
        freePolynomial(p2);
        freePolynomial(p3);
        freePolynomial(p4);
        freePolynomial(p5);
        freePolynomial(p6);
        freePolynomial(p7);
        return 0;
    }
    
    freePolynomial(p1);
    freePolynomial(p2);
    freePolynomial(p3);
    freePolynomial(p4);
    freePolynomial(p5);
    freePolynomial(p6);
    freePolynomial(p7);
    
    return 1;
}

int test_gcdPolynomials() {
    printf("--- Test gcdPolynomials ---\n");
    
    error_code err = NONE;
    
    Polynomial *p1 = parsePolynomial("1 2 -1 0", &err),
                *p2 = parsePolynomial("1 3 1 2 1 1 1 0", &err),
                *p3 = parsePolynomial("1 1 1 0", &err),
                *p4;
    if(!p1 || !p2 || !p3) {
        printError(stdout, &err, NULL);
        freePolynomial(p1);
        freePolynomial(p2);
        freePolynomial(p3);
        return 0;
    }

    p4 = gcdPolynomials(p1->monomials, p2->monomials, &err);
    if(!p4) {
        printError(stdout, &err, NULL);
        freePolynomial(p1);
        freePolynomial(p2);
        freePolynomial(p3);
        return 0;
    }
    
    if(!comparePolynomials(p3, p4)) {
        printf("  Wrong gcd.\n");
        freePolynomial(p1);
        freePolynomial(p2);
        freePolynomial(p3);
        freePolynomial(p4);
        return 0;
    }

    freePolynomial(p1);
    freePolynomial(p2);
    freePolynomial(p3);
    freePolynomial(p4);
    
    return 1;
}
    
    
