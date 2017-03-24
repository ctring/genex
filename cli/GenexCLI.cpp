#include <iostream>
#include <vector>
#include <map>
#include <boost/tokenizer.hpp>
#include <readline/readline.h>
#include <readline/history.h>

#include "Command.hpp"
#include "GenexAPI.hpp"
#include "Exception.hpp"

#include "config.hpp"

static genex::GenexAPI genexAPI;

MAKE_COMMAND(LoadData,
  {
    if (args.size() > 5) {
      std::cout << "Error: Too many arguments" << std::endl;
      return false;
    }

    std::string filePath = args[1];
    int maxNumRow = args.size() > 2 ? std::stoi(args[2]) : 0;
    int startCol  = args.size() > 3 ? std::stoi(args[3]) : 0;
    std::string separators = args.size() > 4 ? args[4] : " ";

    int id;
    try {
      id = genexAPI.loadDataset(filePath, maxNumRow, startCol, separators);
    } catch (genex::GenexException& e)
    {
      std::cout << "Error: " << e.what() << std::endl;
    }
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
        std::cout << "Error: cannot find help for command: " << args[1] << std::endl;
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
      std::cout << "Error: cannot find command: " << args[0] << std::endl;
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
