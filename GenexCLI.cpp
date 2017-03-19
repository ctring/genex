#include <iostream>
#include "config.hpp"
#include "DistanceManager.cpp"

int main (int argc, char *argv[])
{
#ifdef SINGLE_PRECISION
  std::cout << "Single precision!\n";
#endif
  std::cout << "Unimplemented!\n";

  const DistanceManager* m = DistanceManager::getInstance();
  return 1;
}
