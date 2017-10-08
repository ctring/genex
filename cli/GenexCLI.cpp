#include <chrono>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <map>
#include <vector>
#include <boost/tokenizer.hpp>
#include <readline/readline.h>
#include <readline/history.h>

#include "GenexAPI.hpp"
#include "Command.hpp"
#include "Exception.hpp"

#include "config.hpp"

#define TIME_COMMAND(_command)                                          \
  chrono::time_point<chrono::system_clock> __start_time, __end_time;    \
  __start_time = chrono::system_clock::now();                           \
  _command                                                              \
  __end_time = chrono::system_clock::now();                             \
  if (gTimerEnabled)                                                    \
  {                                                                     \
    chrono::duration<float> elapsed_seconds = __end_time - __start_time;\
    cout << "Command executed in ";                                     \
    cout << setprecision(4) << elapsed_seconds.count() << "s" << endl;  \
  }

using namespace std;

genex::GenexAPI gGenexAPI;
bool gTimerEnabled = true;

bool tooFewArgs(const vector<string>& args, int limit)
{
  if (args.size() < limit) {
    cout << "Error! Too few arguments" << endl;
    return true;
  }
  return false;
}

bool tooManyArgs(const vector<string>& args, int limit)
{
  if (args.size() > limit) {
    cout << "Error! Too many arguments" << endl;
    return true;
  }
  return false;
}

/**************************************************************************
 * HOW TO CREATE A NEW COMMAND
 *
 * Step 1: Define a new command using the MAKE_COMMAND macro
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
 *
 * This macro creates a class Cmd<command_name> and its object in a variable
 * called cmd<command_name>. This variable is used in step 2.
 **************************************************************************/

MAKE_COMMAND(LoadDataset,
  {
    if (tooFewArgs(args, 2) || tooManyArgs(args, 5))
    {
      return false;
    }

    string filePath = args[1];
    int maxNumRow = args.size() > 2 ? stoi(args[2]) : 0;
    int startCol  = args.size() > 3 ? stoi(args[3]) : 0;
    string separators = args.size() > 4 ? args[4] : " ";

    genex::dataset_info_t info;
    
    info = gGenexAPI.loadDataset(filePath, maxNumRow, startCol, separators);

    cout << "Dataset loaded                         " << endl
              << "  Name:        " << info.name       << endl
              << "  ID:          " << info.id         << endl
              << "  Item count:  " << info.itemCount  << endl
              << "  Item length: " << info.itemLength << endl;

    return true;
  },

  "Load a dataset to the memory",

  "Dataset are text files with table-like format, such as comma-separated  \n"
  "values files.                                                           \n"
  "                                                                        \n"
  "Usage: load <filePath> [<maxNumRow> <startCol> <separators>]            \n"
  "  filePath  - Path to a text file containing the dataset                \n"
  "  maxNumRow - Maximum number of rows will be read from the file. If this\n"
  "              number is non-positive or the number of actual line is    \n"
  "              smaller than this number, all lines are read. (default: 0)\n"
  "  startCol  - Omit all columns before this column. (default: 0)         \n"
  "  seprators - A list of characters used to separate values in the file  \n"
  "              (default: <space>)                                          "
  )

MAKE_COMMAND(UnloadDataset,
  {
    if (tooFewArgs(args, 2) || tooManyArgs(args, 5))
    {
      return false;
    }

    int index = stoi(args[1]);

    gGenexAPI.unloadDataset(index);

    cout << "Dataset " << index << " is unloaded" << endl;
    return true;
  },

  "Unload a dataset from the memory",

  "Usage: unload <dataset_index>                               \n"
  "  dataset_index  - Index of the dataset being unloaded. Use \n"
  "                   'list dataset' to retrieve the list of   \n"
  "                   loaded datasets.                           "
  )

MAKE_COMMAND(List,
  {
    if (tooFewArgs(args, 2) || tooManyArgs(args, 2))
    {
      return false;
    }

    if (args[1] == "dataset")
    {
      vector<genex::dataset_info_t> infos = gGenexAPI.getAllDatasetInfo();
      cout << "There are " << infos.size() << " loaded datasets" << endl << endl;
      for (const auto& i : infos)
      {
        cout << "  " << setw(4) << i.id << " " << i.name;
        cout << "\t" << setw(10) << (i.isNormalized ? "Normalized" : "");
        cout << "\t" << setw(10) << (i.isGrouped ? "Grouped" : "");
        cout << endl;
      }
    }
    else if (args[1] == "distance") {
      vector<genex::distance_info_t> infos = gGenexAPI.getAllDistanceInfo();
      for (const auto& i : infos)
      {
        cout << " " << setw(10) << i.name << "\t" << i.description << endl;
      }
    }
    else
    {
      cout << "Error! Unknown object: " << args[1] << endl;
      return false;
    }
    return true;
  },

  "List loaded dataset or available distance",

  "Usage: list dataset|distance"
  )

MAKE_COMMAND(Timer,
  {
    if (tooFewArgs(args, 1) || tooManyArgs(args, 2))
    {
      return false;
    }
    if (args.size() == 1)
    {
      cout << "Timer is " << (gTimerEnabled ? "ON" : "OFF") << endl;
    }
    else
    {
      if (args[1] == "on")
      {
        gTimerEnabled = true;
        cout << "Timer is ON" << endl;
      }
      else if (args[1] == "off")
      {
        gTimerEnabled = false;
        cout << "Timer is OFF" << endl;
      }
      else
      {
        cout << "Error! Argument for timer must be 'on' or 'off'" << endl;
        return false;
      }
    }
    return true;
  },

  "Turn timer on or off",

  "When timer is turned on, each command is timed and reported at  \n"
  "the end of its execution. If this command is called without     \n"
  "an additional argument, the current state of timer is printed.  \n"
  "                                                                \n"
  "Usage: timer [on|off]                                             ")

MAKE_COMMAND(GroupDataset,
  {
    if (tooFewArgs(args, 3) || tooManyArgs(args, 4))
    {
      return false;
    }

    int index = stoi(args[1]);
    genex::data_t threshold = stod(args[2]);

    string distance_name = "euclidean";

    // if distance option is present
    if (args.size() == 4)
    {
      distance_name = args[3];
    }
    int count = -1;
    TIME_COMMAND(
      count = gGenexAPI.groupDataset(index, threshold, distance_name);
    )

    cout << "Dataset " << index << " is now grouped" << endl;
    cout << "Number of Groups: " << count << endl;
    return true;
  },

  "Group a dataset in memory",

  "Usage: group <dataset_index> <threshold> [<distance>]          \n"
  "  dataset_index   - Index of the dataset being unloaded. Use   \n"
  "                    'list dataset' to retrieve the list of     \n"
  "                    loaded datasets.                           \n"
  "  threshold       - Threshold for grouping.                    \n"
  "  distance        - The string identifier of a distance. This  \n"
  "                    string must not end with '_dtw'. Use       \n"
  "                    'list distance' to retrieve the list of    \n"
  "                    loaded distance. Default to euclidean.     \n"
  )

MAKE_COMMAND(SaveGroup,
  {
    if (tooFewArgs(args, 3) || tooManyArgs(args, 4))
    {
      return false;
    }

    int index = stoi(args[1]);
    bool groupSizeOnly = false;
    if (args.size() == 4) {
      groupSizeOnly = stoi(args[3]);
    }

    gGenexAPI.saveGroup(index, args[2], groupSizeOnly);
    cout << "Saved groups of dataset " << index << " to " << args[2] << endl;

    return true;
  },

  "Save groups of a grouped dataset",

  "Usage: saveGroup <dataset_index> <path> [<groupSizeOnly>]            \n"
  "  dataset_index   - Index of the dataset whose groups will be saved. \n"
  "  path            - Where to save the groups.                        \n"
  "  groupSizeOnly   - If set to 1, only the sizes of groups are saved  \n"
  "                    Default is 0.                                    \n"
  )

MAKE_COMMAND(LoadGroup,
  {
    if (tooFewArgs(args, 3) || tooManyArgs(args, 3))
    {
      return false;
    }

    int index = stoi(args[1]);

    int numGroups = gGenexAPI.loadGroup(index, args[2]);
    cout << numGroups << " groups loaded for dataset " << index;

    return true;
  },

  "Load saved groups to a compatible dataset",

  "A dataset is compatible with a saved group file is when the item     \n"
  "count and item length is the same.                                   \n"
  "                                                                     \n"
  "Usage: loadGroup <dataset_index> <path>                              \n"
  "  dataset_index   - Index of the dataset whose groups will be saved. \n"
  "  path            - Where to save the groups.                        \n"
  )

MAKE_COMMAND(NormalizeDataset,
  {
    if (tooFewArgs(args, 2) || tooManyArgs(args, 2))
    {
      return false;
    }

    int index = stoi(args[1]);

    gGenexAPI.normalizeDataset(index);

    cout << "Dataset " << index << " is now normalized" << endl;
    return true;
  },

  "Normalize a dataset. (Warning: this operation cannot be undone)",

  "Usage: normalize <dataset_index> \n"
  "  dataset_index   - Index of the dataset to be normalized"
)

MAKE_COMMAND(Match,
  {
    if (tooFewArgs(args, 4) || tooManyArgs(args, 8))
    {
      return false;
    }

    int db_index = stoi(args[1]);
    int  q_index = stoi(args[2]);
    int ts_index = stoi(args[3]);
    int start = 0;
    int end = -1;

    if (args.size() > 4)
    {
      start = stoi(args[4]);
      end = stoi(args[5]);
    }

    genex::candidate_time_series_t best;
    TIME_COMMAND(
      if (end == -1)
      {
        best = gGenexAPI.getBestMatch(db_index, q_index, ts_index);
      }
      else
      {
        best = gGenexAPI.getBestMatch(db_index, q_index, ts_index, start, end);
      }
    )

    cout << "Best Match is timeseries " << best.data.getIndex()
    << " starting at " << best.data.getStart()
    << " with length " << best.data.getLength()
    << ". Distance = " << best.dist    
    << endl;

    return true;
  },

  "Find the best match of a time series",

  "Usage: match <target_dataset_idx> <q_dataset_idx> <ts_index> [<start> <end>] \n"
  "  dataset_index   - Index of loaded dataset to get the result from. \n"
  "                    Use 'list dataset' to retrieve the list of     \n"
  "                    loaded datasets.                           \n"
  "  q_dataset_idx   - Same as dataset_index, except for the query \n"
  "  ts_index        - Index of the query \n"
  "  start           - The start location of the query in the timeseries \n"
  "  end             - The end location of the query in the timeseries (this point is not included)\n"
  )

MAKE_COMMAND(KNN,
  {
    if (tooFewArgs(args, 5) || tooManyArgs(args, 9))
    {
      return false;
    }

    int db_index = stoi(args[1]);
    int  q_index = stoi(args[2]);
    int ts_index = stoi(args[3]);
    int start = 0;
    int end = -1;
    int k;

    if (args.size() > 5)
    {
      start = stoi(args[4]);
      end = stoi(args[5]);
      k = stoi(args[6]);
    }
    else
    {
      k = stoi(args[4]);  
    }

    vector<genex::TimeSeries> results;
    TIME_COMMAND(
      if (end == -1)
      {
        results = gGenexAPI.kNN(db_index, q_index, ts_index, k);
      }
      else
      {
        results = gGenexAPI.kNN(db_index, q_index, ts_index, start, end, k); 
      }
    )

    for (int i = 0; i < results.size(); i++) {
      cout << "Timeseries " << results[i].getIndex()
                << " at " << results[i].getStart()
                << " with length " << results[i].getLength()
                << endl; 
    }
    return true;
  },

  "Perform knn on a time series",
  
    "Usage: knn <target_dataset_idx> <q_dataset_idx> <ts_index> [<start> <end>] <k> \n"
    "  dataset_index   - Index of loaded dataset to get the result from. \n"
    "                    Use 'list dataset' to retrieve the list of     \n"
    "                    loaded datasets.                           \n"
    "  q_dataset_idx   - Same as dataset_index, except for the query \n"
    "  ts_index        - Index of the query \n"
    "  start           - The start location of the query in the timeseries \n"
    "  end             - The end location of the query in the timeseries (this point is not included)\n"
    "  k               - The number of neigbors \n"
    )

/**************************************************************************
 * Step 2: Add the Command object into the commands map
 *
 * The key is a string which is also the string that is used to match with
 * user input. The value is the cmd<command_name> variable created in step 1.
 **************************************************************************/

map<string, Command*> commands = {
  {"load", &cmdLoadDataset},
  {"unload", &cmdUnloadDataset},
  {"list", &cmdList},
  {"timer", &cmdTimer},
  {"group", &cmdGroupDataset},
  {"saveGroup", &cmdSaveGroup},
  {"loadGroup", &cmdLoadGroup},  
  {"normalize", &cmdNormalizeDataset},
  {"match", &cmdMatch},
  {"knn", &cmdKNN}
};

/**************************************************************************/
/**************************************************************************/

#define COUT_HELP_ALIGNMENT 15

const string HELP_SUMMARY = "Retrieve a list of commands or get help for a command";
const string HELP_HELP    = "Usage: help [<command_name>]                                \n"
                                 "  command_name - Name of command to retrieve help about. If \n"
                                 "                 not specified, a list of available commands\n"
                                 "                 is shown instead.                            ";
const string EXIT_SUMMARY = "Terminate the program";
const string EXIT_HELP    = "Usage: Can use either 'exit' or 'quit'";

void showHelp(const string& command_name)
{
  if (command_name == "help")
  {
    cout << HELP_SUMMARY << endl << HELP_HELP << endl;
  }
  else if (command_name == "exit" || command_name == "quit")
  {
    cout << EXIT_SUMMARY << endl << EXIT_HELP << endl;
  }
  else if (commands.find(command_name) != commands.end())
  {
    Command* cmd = commands[command_name];
    cout << cmd->getSummary() << endl << cmd->getHelp() << endl;
  }
  else
  {
    cout << "Error! Cannot find help for command: " << command_name << endl;
  }
}

void showAllHelps()
{
  cout << "Use 'help <command>' to see help for a command" << endl << endl;

  cout << setw(COUT_HELP_ALIGNMENT);
  cout << "help" << HELP_SUMMARY << endl;

  cout << setw(COUT_HELP_ALIGNMENT);
  cout << "exit|quit " << EXIT_SUMMARY << endl;

  cout << endl;
  for (const auto& cmd : commands)
  {
    cout << setw(COUT_HELP_ALIGNMENT);
    cout << cmd.first << cmd.second->getSummary() << endl;
  }
}

bool processLine(const string& line)
{
  boost::char_separator<char> sep(" ");
  typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
  tokenizer tokens(line, sep);
  vector<string> args (tokens.begin(), tokens.end());

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
    if (args.size() == 1)
    {
      showAllHelps();
    }
    else if (args.size() == 2)
    {
      showHelp(args[1]);
    }
    else
    {
      cout << "Error! Too many arguments for 'help'" << endl;
    }
  }
  else
  {
    if (commands.find(args[0]) == commands.end())
    {
      cout << "Error! Cannot find command: " << args[0] << endl;
    }
    else {
      commands[args[0]]->doCommand(args);
    }
  }

  return false;
}

bool wantToQuitByEOF()
{
  cout << endl;
  char* raw_line = nullptr;
  do
  {
    delete raw_line;
    raw_line = readline("Do you really want to exit ([y]/n)? ");
    if (raw_line == nullptr || strcmp(raw_line, "y") == 0 || strcmp(raw_line, "") == 0)
    {
      return true;
    }
  }
  while (strcmp(raw_line, "n") != 0);
  return false;
}

int main (int argc, char *argv[])
{
  bool quit = false;
  char* raw_line;

  // Align left
  cout << left;
  cout << "Welcome to GENEX!\n"
               "Use 'help' to see the list of available commands." << endl;

  while (true)
  {
    quit = false;
    raw_line = readline(">> ");

    if (raw_line == nullptr) // Ctrl-D is hit or EOF
    {
      quit = wantToQuitByEOF();
    }
    else
    {
      if (strlen(raw_line) > 0)
      {
        add_history(raw_line);
      }
      string line = string(raw_line);
      delete raw_line;
      try
      {
        quit = processLine(line);
      }
      catch (genex::GenexException& e)
      {
        cout << "Error! " << e.what() << endl;
      }
      catch (logic_error& e)
      {
        cout << "Error! Cannot convert some value to numeric" << endl;
      }
      catch (...)
      {
        cout << "Error! Unknown error" << endl;
        quit = true;
      }
      cout << endl;
    }

    if (quit)
    {
      break;
    }
  }
  return 0;
}
