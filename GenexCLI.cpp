#include <iostream>
#include "config.hpp"
#include "distance/DistanceManager.cpp"

int main (int argc, char *argv[])
{
  using namespace genex;

#ifdef SINGLE_PRECISION
  std::cout << "Single precision!\n";
#endif
  std::cout << "Unimplemented!\n";

  const DistanceManager* m = DistanceManager::getInstance();
  return 1;
}
