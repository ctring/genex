#include <cstdio>
#include <iostream>

#include "ExampleClass.hpp"

int main (int argc, char *argv[])
{
  fprintf(stdout, "Testing build & make.\n");
#ifdef FIND_EXACT
  fprintf(stdout, "Options working\n");
#endif

  Rectangle rect;
  rect.set_values (3,4);
  std::cout << "area: " << rect.area() << std::endl;

  return 0;
}
