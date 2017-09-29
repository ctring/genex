#include "lib/util.h"
#include <stdlib.h>
#include <stdio.h>
#include <cmath>

/// Print function for debugging
void printArray(double *x, int len)
{
    for(int i=0; i<len; i++)
        printf(" %6.2lf",x[i]);
    printf("\n");
}

/// If expected error happens, teminated the program.
void error(int id)
{
    switch(id) {
    case 1:
        printf("ERROR : Memory can't be allocated!!!\n\n");
	break;
    case 2:
        printf("ERROR : File not Found!!!\n\n");
	break;
    case 3:
        printf("ERROR : Can't create Output File!!!\n\n");
	break;
    case 4:
        printf("ERROR : Invalid Number of Arguments!!!\n");
        printf("Command Usage:  UCR_DTW.exe  data-file  query-file   m   R\n\n");
        printf("For example  :  UCR_DTW.exe  data.txt   query.txt   128  0.05\n");
	break;
    }

    exit(EXIT_FAILURE);
}

int Index::comp(const void *a, const void* b)
{
    Index* x = (Index*)a;
    Index* y = (Index*)b;

    return (*x).diff(*y);
}
int Index::diff(Index &other)
{
    return std::abs(other.value) - std::abs(value);
}
