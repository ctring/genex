#include <iostream>
#include <readline/readline.h>
#include <readline/history.h>

#include "config.hpp"

int main (int argc, char *argv[])
{
  char* line = readline("> ");
  std::cout << std::string(line) << std::endl;
  return 0;
}
