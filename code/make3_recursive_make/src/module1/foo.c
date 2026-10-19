// src/module1/foo.c

#include <stdio.h>
#include "foo_internal.h"
#include "module1/foo.h"

void foo_internal(void) {
    printf("foo_internal called \n");
}

void foo_function(void) {
    printf("foo_function called \n");
}