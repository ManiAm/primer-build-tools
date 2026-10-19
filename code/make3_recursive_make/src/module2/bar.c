// src/module2/bar.c

#include <stdio.h>
#include "bar_internal.h"
#include "module2/bar.h"

void bar_internal(void) {
    printf("bar_internal called \n");
}

void bar_function(void) {
    printf("bar_function called \n");
}