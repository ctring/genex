#ifndef COMMAND_H
#define COMMAND_H

#include <vector>
#include <string>
#include <iostream>

class Command
{
public:
  virtual bool doCommand(const std::vector<std::string>&  args) = 0;
  virtual const std::string& getSummary() { return summary; }
  virtual const std::string& getHelp() { return help; };

protected:
  std::string summary;
  std::string help;
};

#define MAKE_COMMAND(_name, _content, _summary, _help)                  \
  class Cmd##_name : public Command {                                   \
  public:                                                               \
    Cmd##_name() { help = _help; summary = _summary; }                  \
    bool doCommand(const std::vector<std::string>&  args) _content      \
  } cmd##_name;

#endif // COMMAND_H