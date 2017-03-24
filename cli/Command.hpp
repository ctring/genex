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

/**
 * This macro creates a class Cmd<command_name> and its object cmd<command_name>
 *
 * MAKE_COMMAND(<command_name>, <code>, <help_summary>, <help>)
 *  <command_name> - name of the command
 *  <code>         - the code being executed when this command is called.
 *                   Arguments are put in a vector of string call 'args'.
 *                   This code should return true if the command is carried
 *                   out successfully and false otherwise.
 *  <help_summary> - a short text line describing the command. This is
 *                   showed next to the command name when 'help' is called
 *  <help>         - a long text describe the command in details. This is
 *                   showed when 'help <command_name>' is called
 */
#define MAKE_COMMAND(_name, _content, _summary, _help)                  \
  class Cmd##_name : public Command {                                   \
  public:                                                               \
    Cmd##_name() { help = _help; summary = _summary; }                  \
    bool doCommand(const std::vector<std::string>&  args) _content      \
  } cmd##_name;

#endif // COMMAND_H