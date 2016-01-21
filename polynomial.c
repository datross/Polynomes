/* polynomial.c */

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "polynomial.h"

void printError(FILE * stream, error_code * err, char * details) {
    if(err && *err != none && stream) {
        fprintf(stream, "ERROR %d : ", *err);
        switch(*err) {
            case mem_alloc:
                fprintf(stream, "memory allocation failed.");
                break;
            case null_ptr:
                fprintf(stream, "an argument is NULL and should not be.");
                break;
            case poly_format:
                fprintf(stream, "polynomial string has not a valid format.");
                break;
            case division_by_zero:
                fprintf(stream, "division by zero.");
                break;
            default:
                fprintf(stream, "unknown error.");
                break;
        }
        if(details) 
            fprintf(stream, " DETAILS : %s", details);
        fprintf(stream, "\n");
    }
}

Monomial * createMonomial(double coefficient, long degree, error_code * err) {
    Monomial * monomial = malloc(sizeof(Monomial));
    if(!monomial) {
        if(err) *err = mem_alloc;
        return NULL;
    }

    monomial->coefficient = coefficient;
    monomial->degree = degree;
    monomial->next = NULL;

    return monomial;
}

Polynomial * createNullPolynomial(error_code * err) {
    Polynomial * poly = malloc(sizeof(Polynomial));
    if(!poly) {
        if(err) *err = mem_alloc;
        return NULL;
    }
    
    if(!(poly->monomials = createMonomial(0., 0, err))) {
    	return NULL;
    }
    
    poly->degree = 0;
    
    return poly;
}

void freeList(List * list) {
    List * next = list;
    while(list) {
        next = list->next;
        free(list);
        list = next;
    }
}

void freePolynomial(Polynomial * poly) {
    if(poly) {
        freeList(poly->monomials);
        free(poly);
    }
}

List * copyList(List * src, error_code * err) {
    if(!src)
        return NULL;
        
    List *dest = createMonomial(src->coefficient, src->degree, err);
    if(!dest)
        return NULL;
    List * tail = dest;
    for(src = src->next; src; src = src->next) {
        tail->next = createMonomial(src->coefficient, src->degree, err);
        if(!(tail->next)) {
            freeList(dest);
            return NULL;
        }
        tail = tail->next;
    }
    return dest;
}

Polynomial * copyPolynomial(Polynomial * src, error_code * err) {
    if(!src) 
        return NULL;
    Polynomial * dest = malloc(sizeof(Polynomial));
    if(!dest) {
        if(err) *err = mem_alloc;
        return NULL;
    }
    dest->monomials = copyList(src->monomials, err);
    dest->degree = src->degree;

    return dest;
}

void printPolynomial(FILE * stream, Polynomial * poly, error_code * err) {
	if(!poly || !(poly->monomials) || !stream) {
		if(err) *err = null_ptr;
		return;
	}
	
	Monomial * current_monomial = poly->monomials;
	
    fprintf(stream, "degree : %ld | (", poly->degree);
	for(; current_monomial; current_monomial = current_monomial->next) {
		fprintf(stream, "(%lf, %ld)", current_monomial->coefficient, current_monomial->degree);
		if(current_monomial->next)
			fprintf(stream, ", ");
		else
			fprintf(stream, ")");
	}
    fprintf(stream, "\n");
}

Polynomial * parsePolynomial(char * poly_str, error_code * err) {
	if(!poly_str) {
        if(err) *err = null_ptr;
        return NULL;
    }
    
    Polynomial * poly;
	if(!(poly = createNullPolynomial(err)))
		return NULL;
	
	char * end;
	double coefficient;
	long degree;
    Monomial * buffer;
	while(1) {
		coefficient = strtod(poly_str, &end);
		if(end == poly_str) { // if conversion doesn't work, this monomial doesn't exist, so it is the end
            return poly;
		}
		poly_str = end;
		degree = strtol(poly_str, &end, 10);
		if(end == poly_str || degree < 0) { // if this conversion doesn't work, there is a problem in "pol_str" format
            if(err) *err = poly_format;
			return poly;
		}
		poly_str = end;
		
        if(!(buffer = createMonomial(coefficient, degree, err)))
            return NULL;
        if(err) *err = none;
		addPolynomials(poly, buffer, err);
        if(err && *err != none)
            return NULL;
	}
}

void addPolynomials(Polynomial * poly, List * list_to_add, error_code * err) {
    if(!poly || !(poly->monomials)) {
        if(err) *err = null_ptr;
        return;
    }
    
    Monomial * current_monomial = poly->monomials;
    Monomial * prec = NULL;
    for(; list_to_add; list_to_add = list_to_add->next) {
        if(list_to_add->coefficient != 0.) {
            for(; current_monomial && current_monomial->degree > list_to_add->degree;  prec = current_monomial, current_monomial = current_monomial->next);
            
            if(current_monomial->degree == list_to_add->degree) {
                if(current_monomial->coefficient + list_to_add->coefficient == 0. && list_to_add->degree != 0) {
                    if(prec) {
                        prec->next = current_monomial->next;
                        free(current_monomial);
                        current_monomial = current_monomial->next;
                    } else {
                        poly->monomials = current_monomial->next;
                        free(current_monomial);
                        current_monomial = current_monomial->next;
                    }
                } else {
                    current_monomial->coefficient += list_to_add->coefficient;
                }
            } else {
                if(prec) {
                    prec->next = createMonomial(list_to_add->coefficient, list_to_add->degree, err);
                    if(!(prec->next))
                        return;
                    prec->next->next = current_monomial;
                    prec = prec->next;
                } else {
                    poly->monomials = createMonomial(list_to_add->coefficient, list_to_add->degree, err);
                    if(!(poly->monomials))
                        return;
                    poly->monomials->next = current_monomial;
                    prec = poly->monomials;
                }
            }   
        }  
    }
    poly->degree = poly->monomials->degree;
}

void differentiatePolynomial(Polynomial * poly, error_code * err) {
    if(!poly || !(poly->monomials)) {
        if(err) *err = null_ptr;
        return;
    }
    
    Monomial * current_monomial = poly->monomials;
    for(; current_monomial; current_monomial = current_monomial->next) {
        current_monomial->coefficient *= (double)current_monomial->degree;
        if(current_monomial->degree == 1) {
            --current_monomial->degree;
            free(current_monomial->next);
            current_monomial->next = NULL;
        }
        else if(current_monomial->degree > 1)
            --current_monomial->degree;
    }
    if(poly->degree > 0)
        --poly->degree;
}

void multiplyPolynomials(Polynomial * poly, List * factor, error_code * err) {
    if(!poly || !(poly->monomials) || !factor) {
        if(err) *err = null_ptr;
        return;
    }

    Polynomial * result = createNullPolynomial(err);
    if(!result) 
        return;
    List * temp = NULL;
    
    Monomial * current_monomial_poly, *current_monomial_temp;
    for(; factor; factor = factor->next) {
        if(factor->coefficient != 0.) {
            temp = NULL;
            current_monomial_temp = temp;
            current_monomial_poly = poly->monomials;
            for(; current_monomial_poly; current_monomial_poly = current_monomial_poly->next) {
                if(temp == NULL) {
                    temp = createMonomial(factor->coefficient * current_monomial_poly->coefficient, factor->degree + current_monomial_poly->degree, err);
                    if(!temp)
                        return;
                    current_monomial_temp = temp;
                } else {            
                    current_monomial_temp->next = createMonomial(factor->coefficient * current_monomial_poly->coefficient, factor->degree + current_monomial_poly->degree, err);
                    if(!(current_monomial_temp->next))
                        return;
                    current_monomial_temp = current_monomial_temp->next;
                }
            }
            addPolynomials(result, temp, err);
            
            freeList(temp);
        }
    }
    
    freeList(poly->monomials);
    poly->monomials = result->monomials;
    poly->degree = result->degree;
    
    free(result);
}

void powerPolynomial(Polynomial * poly, unsigned int n, error_code * err) {
    if(!poly || !(poly->monomials)) {
        if(err) *err = null_ptr;
        return;
    }
    
    Polynomial * result = createNullPolynomial(err);
    if(!result)
        return;
    Monomial * unit = createMonomial(1, 0, err);
    if(!unit)
        return;
    addPolynomials(result, unit, err);
    
    while(n != 0) {
        if(n & 1) {
            n = (n-1) / 2;
            multiplyPolynomials(result, poly->monomials, err);
        } else {
            n /= 2;
        }
        multiplyPolynomials(poly, poly->monomials, err);
    }
    
    freeList(poly->monomials);
    poly->monomials = result->monomials;
    poly->degree = result->degree;
    
    free(result);
}

double evaluatePolynomial(Polynomial * poly, double x, error_code * err) {
    if(!poly || !(poly->monomials)) {
        if(err) *err = null_ptr;
        return 0.;
    }
    
    Monomial * current_monomial = poly->monomials;
    double result = 0.;
    
    for(; current_monomial; current_monomial = current_monomial->next) {
        result += current_monomial->coefficient * pow(x, current_monomial->degree);
    }
    
    return result;
}

double evaluatePolynomialHorner(Polynomial * poly, double x, error_code * err) {
    if(!poly || !(poly->monomials)) {
        if(err) *err = null_ptr;
        return 0.;
    }
    
    double result = 0.;
    long degree_prec = 0;
    Monomial *current_monomial = poly->monomials;
    while(current_monomial) {
        result = (result * pow(x, degree_prec - current_monomial->degree)) + current_monomial->coefficient;
        degree_prec = current_monomial->degree;
        current_monomial = current_monomial->next;
    }
    
    return result;
}

void dividePolynomials(List * dividend, List * divisor, Polynomial * quotient, Polynomial * remainder, error_code * err) {
    if(!dividend || !divisor) {
        if(err) *err = null_ptr;
        return;
    }
    
    if(!(divisor->coefficient)) {
        if(err) *err = division_by_zero;
        return;
    }        

    if(!quotient && !remainder)
        return;
        
    Polynomial *tmp_quotient = createNullPolynomial(err), 
               *tmp_remainder = malloc(sizeof(Polynomial));
    if(!tmp_quotient || !tmp_remainder) {
        freePolynomial(tmp_quotient);
        freePolynomial(tmp_remainder);
        if(err && *err == none) *err = mem_alloc;
        return;
    }
    tmp_remainder->degree = dividend->degree;
    tmp_remainder->monomials = NULL;
    tmp_remainder->monomials = copyList(dividend, err);
    if(!(tmp_remainder->monomials)) {
        freePolynomial(tmp_quotient);
        free(tmp_remainder);
        return;
    }
    
    Monomial * tmp = NULL;
    Polynomial * tmp_poly;
    while(tmp_remainder->degree >= divisor->degree && tmp_remainder->monomials->coefficient != 0) {
        tmp = createMonomial(tmp_remainder->monomials->coefficient / divisor->coefficient, tmp_remainder->degree - divisor->degree, err);
        addPolynomials(tmp_quotient, tmp, err);
        if(!tmp) {
            freePolynomial(tmp_quotient);
            freePolynomial(tmp_remainder);
            return;
        }
        tmp_poly = createNullPolynomial(err);
        if(!tmp_poly) {
            freePolynomial(tmp_quotient);
            freePolynomial(tmp_remainder);
            freeList(tmp);
            return;
        }
        tmp_poly->monomials->coefficient = -1;
        multiplyPolynomials(tmp_poly, divisor, err);
        multiplyPolynomials(tmp_poly, tmp, err);
        addPolynomials(tmp_remainder, tmp_poly->monomials, err);
        freePolynomial(tmp_poly);
        free(tmp);
    }
    
    if(quotient) {
        quotient->degree = tmp_quotient->degree;
        freeList(quotient->monomials);
        quotient->monomials = tmp_quotient->monomials;
        free(tmp_quotient);
    } else {
        freePolynomial(tmp_quotient);
    }
    if(remainder) {
        remainder->degree = tmp_remainder->degree;
        freeList(remainder->monomials);
        remainder->monomials = tmp_remainder->monomials;
        free(tmp_remainder);
    } else {
        freePolynomial(tmp_remainder);
    }
}

Polynomial * gcdPolynomials(List * a, List * b, error_code * err) {
    if(!a || !b) {
        if(err) *err = null_ptr;
        return NULL;
    }
    
    Polynomial * r = createNullPolynomial(err);
    List * tmp_a = copyList(a, err),
         * tmp_b = copyList(b, err);
    if(!r || !tmp_a || !tmp_b) {
        freePolynomial(r);
        freeList(tmp_a);
        freeList(tmp_b);
        return NULL;
    }
    
    while(tmp_b->coefficient) {
        dividePolynomials(tmp_a, tmp_b, NULL, r, err);
        freeList(tmp_a);
        tmp_a = tmp_b;
        tmp_b = copyList(r->monomials, err);
        if(!tmp_b) {
            freePolynomial(r);
            freeList(tmp_a);
            freeList(tmp_b);
            return NULL;
        }
    }
    
    r->degree = tmp_a->degree;
    freeList(r->monomials);
    r->monomials = tmp_a;
    
    Monomial factor;
    if(r->monomials->coefficient)
        factor.coefficient = 1. / r->monomials->coefficient;
    factor.degree = 0;
    factor.next = NULL;
    
    multiplyPolynomials(r, &factor, err);
    
    freeList(tmp_b);
    
    return r;
}
