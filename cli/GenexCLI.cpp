#include <iostream>
#include <boost/tokenizer.hpp>
#include <readline/readline.h>
#include <readline/history.h>

#include "config.hpp"

bool processLine(const std::string& line)
{
  boost::char_separator<char> sep(" ");
  typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
  tokenizer tokens(line, sep);

  tokenizer::iterator tok_iter = tokens.begin();
  if (tok_iter == tokens.end())
  {
    return false;
  }
  if (*tok_iter == "quit")
  {
    return true;
  }
  for (; tok_iter != tokens.end(); tok_iter++)
  {
    std::cout << (*tok_iter) << std::endl;
  }
  return false;
}

int main (int argc, char *argv[])
{
  while (true)
  {
    char* raw_line = readline(">> ");
    add_history(raw_line);
    std::string line = std::string(raw_line);
    delete raw_line;

    bool quit = processLine(line);

    if (quit) { break; }
  }
  return 0;
}
