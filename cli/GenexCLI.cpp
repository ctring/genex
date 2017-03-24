#include <iostream>
#include <iomanip>
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
      std::cout << "Error! Too many arguments" << std::endl;
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
      std::cout << "Error! " << e.what() << std::endl;
    }
    std::cout << "Dataset loaded. ID: " << id << std::endl;
    return true;
  },

  "Load a dataset to the memory",

  "This is a help line\n"
  "This is another")

std::map<std::string, Command*> commands = {
  {"load", &cmdLoadData},
};

#define COUT_HELP_ALIGNMENT 15

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
        std::cout << "Error! Cannot find help for command: " << args[1] << std::endl;
      }
      else {
        Command* cmd = commands[args[1]];
        std::cout << cmd->getSummary() << std::endl << cmd->getHelp() << std::endl;
      }
    }
    else {
      for (const auto& cmd : commands)
      {
        std::cout << std::setw(COUT_HELP_ALIGNMENT) << cmd.first << cmd.second->getSummary() << std::endl;
      }
      std::cout << std::setw(COUT_HELP_ALIGNMENT) << "exit|quit " << "Terminate the program" << std::endl;
    }
  }
  else
  {
    if (commands.find(args[0]) == commands.end())
    {
      std::cout << "Error! Cannot find command: " << args[0] << std::endl;
    }
    else {
      commands[args[0]]->doCommand(args);
    }
  }

  return false;
}

int main (int argc, char *argv[])
{
  std::cout << std::left;
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
