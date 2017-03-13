#include <stdio.h>
#include <stdlib.h>

#include "config.h"

int main (int argc, char *argv[])
{
  fprintf(stdout, "Testing build & make.");
  #ifdef FIND_EXACT
  fprintf(stdout, "Options working");
  #endif

  return 0;
}
