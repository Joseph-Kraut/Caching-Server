//
// Created by Joey Kraut on 1/2/22.
//
#include <stdio.h>

#include "helpers.h"

void print_failure(char *test_name)
{
    printf("%s: ", test_name);
    printf("\033[0;31m");
    printf("FAILURE\n");
    printf("\033[0m");
}

void print_success(char *test_name)
{
    printf("%s: ", test_name);
    printf("\033[0;32m");
    printf("SUCCESS\n");
    printf("\033[0m");
}
