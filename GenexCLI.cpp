#include <iostream>
#include "config.hpp"

int main (int argc, char *argv[])
{
#ifdef SINGLE_PRECISION
  std::cout << "Single precision!\n";
#endif
  std::cout << "Unimplemented!\n";
  return 0;
}
