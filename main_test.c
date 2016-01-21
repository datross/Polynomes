/* main_test.c */

#include <stdio.h>
#include "polynomial.h"
#include "tests.h"

int main() {
    if(test_all())
        printf("\nTests succeeded\n");
    else
        printf("\nTests failed\n");
        
    return 0;
}

