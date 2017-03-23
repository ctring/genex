#include <iostream>
#include <vector>
#include <map>
#include <boost/tokenizer.hpp>
#include <readline/readline.h>
#include <readline/history.h>

#include "Command.hpp"
#include "GenexAPI.hpp"

#include "config.hpp"

MAKE_COMMAND(LoadData,
  {
    if (args.size() > 5)
    {
      std::cout << "Too many variables" << std::endl;
      return false;
    }
    std::cout << "Load data called!" << std::endl;
    return true;
  },
  "This is a help line\n"
  "This is another")

std::map<std::string, Command*> commands = {
  {"loadData", &cmdLoadData},
};

bool processLine(const std::string& line)
{
  boost::char_separator<char> sep(" ");
  typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
  tokenizer tokens(line, sep);
  std::vector<std::string> args (tokens.begin(), tokens.end());

  if (args.size() == 0)
  {
    return false;
  }

  if (args[0] == "quit" || args[0] == "exit")
  {
    return true;
  }
  else if (args[0] == "help")
  {
    if (args.size() > 1)
    {
      if (commands.find(args[1]) == commands.end())
      {
        std::cout << "Cannot find help for command: " << args[1] << std::endl;
        return false;
      }
      std::cout << commands[args[1]]->getHelp() << std::endl;
    }
    else {
      // TODO: Show all helps
    }
  }
  else
  {
    if (commands.find(args[0]) == commands.end())
    {
      std::cout << "Cannot find command: " << args[0] << std::endl;
      return false;
    }
    commands[args[0]]->doCommand(args);
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
    std::cout << std::endl;

    if (quit) { break; }
  }
  return 0;
}
