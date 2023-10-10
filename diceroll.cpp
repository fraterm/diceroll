/* diceroll - A C++ Command Line Dice Rolling Program * \
 *     _ _                    _ _
 *    | (_)                  | | |
 *  __| |_  ___ ___ _ __ ___ | | |
 * / _` | |/ __/ _ \ '__/ _ \| | |
 *| (_| | | (_|  __/ | | (_) | | |
 * \__,_|_|\___\___|_|  \___/|_|_|
 * A C++ Command Line Dice Rolling Program
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// standard library includes
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>
// non standard library includes
#include "args.hxx"  // args library for command line parsing https://github.com/taywee/args
#include "srell.hpp" // SRELL std regex like library by a kenotsuki https://akenotsuki.com
// Global variables
const std::string g_appname_string = "diceroll";
const std::string g_version_string = "0.0.1";
const std::string g_author_string = "Written by Michael Costello (fraterm@gmail.com)";
// Global static arguments with g_arg_ prefix
static bool g_arg_debug_flag = false;
static bool g_arg_version_flag = false;
static bool g_arg_separate_flag = false;
static bool g_arg_interactive_flag = false;
//
// function declarations
std::string parseDiceString(std::string);

std::string performDiceRoll(long throws, long dice, long sides);

long rollDie(long sides);

int main(int argc, char *argv[]) {

  // set up the argument parser and arguments
  args::ArgumentParser argsParser(g_appname_string + " help",
                                  g_appname_string + " help for version:" + g_version_string);
  args::HelpFlag help(argsParser, "help", "Display this help menu", {'h', "help"});
  args::Flag debug(argsParser, "debug", "Set the debug flag", {'d', "debug"});
  args::Flag version(argsParser, "version", "Display the version of " + g_appname_string, {'v', "version"});
  args::Flag separate(argsParser, "separate",
                      "Print out the throw number as well as the separate value of each die "
                      "as well as operations and totals",
                      {'s', "separate"});
  args::Flag interactive(argsParser, "interactive",
                         "@TODO interactive mode, diceroll terminal"
                         "interacts with the user in a command line way",
                         {'i', "interactive"});
  args::Group positionals(argsParser, "dice roll string");
  args::PositionalList<std::string> diceStringList(positionals, "dice strings",
                                                   "parseable dice roll description strings");
  args::CompletionFlag completion(argsParser, {"complete"});
  try {
    // parse the args
    argsParser.ParseCLI(argc, argv);
  } catch (args::Help &) {
    // print the pretty parsers autoconstructed help message then cleanly
    // exit -- no need for a help flag even, just default args:: behavior
    std::cout << argsParser;
    return 0;
  } catch (const args::ParseError &e) {
    std::cerr << e.what() << std::endl;
    std::cerr << argsParser;
    return 1; // error
  } catch (const args::ValidationError &e) {

    std::cerr << e.what() << std::endl;
    std::cerr << argsParser;
    return 1; // error
  }
  if (debug) {
    /* do appropriate debug flag stuff */
    g_arg_debug_flag = true;
  }
  if (version) {
    /* do appropriate version flag stuff and then exit */
    std::cout << g_appname_string << " v" << g_version_string << std::endl;
    std::cout << g_author_string << std::endl;
    return 0;
  }
  if (separate) {
    /* do appropriate separate flag stuff */
    g_arg_separate_flag = true;
  }
  if (interactive) {
    /*do appropriate interactive flag stuff*/
    g_arg_interactive_flag = true;
  }
  // done with arg parsing
  // now do things
  for (auto &&diceString : diceStringList) {
    // for now echo entered dice strings handles the positionals
    if (g_arg_debug_flag) {
      std::cout << "running parseDiceString(" << diceString << ")\n";
      std::cout << "have a total diceStringList->size of " << diceStringList->size() << "\n";
    }
    std::string result = parseDiceString(diceString);
    std::cout << result << "\n";
  }
  // if successful when done return 0
  return 0;
}

// function definitions
std::string parseDiceString(std::string diceString) {
  // initialize throw dice and sides values to sensible defaults if not parsed
  long numThrows = 1;
  long numDice = 1;
  long numSides = 6;
  std::string resultString = "";
  std::string smatchFailedReasonString = "unknown"; // @TODO
  // compose a suitable rolldice compatible regex with named groups over
  // several lines
  std::string regexString = "^"
                            "(?<throwgroup>(?<throws>([0-9]){1,})(?<throwch>[x|X]){1}){0,}"
                            "(?<dicegroup>(?<numdice>[0-9]{1,})(?<dicechar>[d|D]){1}(?<sidestype>[0-9]{0,}|%{1})){0,}"
                            "(?<multgroup>(?<multchar>[*]){1}(?<multnum>[0-9]){1,}){0,}"
                            "(?<addsubgroup>(?<addsubchar>[+|-]){1}(?<addsubnum>[0-9]){1,}){0,}"
                            "(?<droplowestgroup>(?<droplowestchar>[s]){1}(?<droplowestnum>[0-9]){1,}){0,}"
                            "$";

  if (g_arg_debug_flag) { // debug printing
    std::cout << "in parseDiceString(" << diceString << ")\n";
  }
  try {
    //
    srell::regex expression(regexString);
    // static match
    srell::smatch staticmatch;
    if (srell::regex_search(diceString, staticmatch, expression)) { // if regex_search successful
      if (g_arg_debug_flag) {                                       // debug printing
        std::cout << "regex_search true: " << std::endl;
        std::cout << "m.size is:" << staticmatch.size() << std::endl;
        for (int i = 0; i < staticmatch.size(); i++) {
          std::cout << "m.str(" << i << "):" << staticmatch.str(i) << std::endl;
        }
        std::cout << "throwgroup: " << staticmatch.str("throwgroup") << " " << std::endl;
        std::cout << "\tthrows: " << staticmatch.str("throws") << " " << std::endl;
        std::cout << "\tthrowch: " << staticmatch.str("throwch") << " " << std::endl;
        std::cout << "dicegroup: " << staticmatch.str("dicegroup") << " " << std::endl;
        std::cout << "\tnumdice: " << staticmatch.str("numdice") << " " << std::endl;
        std::cout << "\tdicechar: " << staticmatch.str("dicechar") << " " << std::endl;
        std::cout << "\tsidestype: " << staticmatch.str("sidestype") << " " << std::endl;
        std::cout << "multgroup: " << staticmatch.str("multgroup") << " " << std::endl;
        std::cout << "\tmultchar: " << staticmatch.str("multchar") << " " << std::endl;
        std::cout << "\tmultnum: " << staticmatch.str("multnum") << " " << std::endl;
        std::cout << "addsubgroup: " << staticmatch.str("addsubgroup") << " " << std::endl;
        std::cout << "\taddsubchar: " << staticmatch.str("addsubchar") << " " << std::endl;
        std::cout << "\taddsubnum: " << staticmatch.str("addsubnum") << " " << std::endl;
        std::cout << "droplowestgroup: " << staticmatch.str("droplowestgroup") << " " << std::endl;
        std::cout << "\tdroplowestchar: " << staticmatch.str("droplowestchar") << " " << std::endl;
        std::cout << "\tdroplowestnum: " << staticmatch.str("droplowestnum") << " " << std::endl;

        std::cout << std::boolalpha; // turn on printing boolean true/false
                                     // rather than 1/0
        std::cout << "empty test on throws:" << staticmatch.str("throws").std::string::empty() << std::endl;
        std::cout << std::noboolalpha; // turn off printing boolean true/false
                                       // rather than 1/0
      }
      if (!staticmatch.str("throws").std::string::empty()) { // throws not empty begin
        numThrows = std::stol(staticmatch.str("throws"));    // assign throws var
        if (g_arg_debug_flag) {
          std::cout << "throws is not empty have set numThrows to " << numThrows << std::endl;
        }
      } // throws not empty end
      if (!staticmatch.str("numdice").std::string::empty()) {
        numDice = std::stol(staticmatch.str("numdice"));
        if (g_arg_debug_flag) {
          std::cout << "numdice is not empty have set numDice to " << numDice << std::endl;
        }
      }
      // sidestype parsed is not empty
      if (!staticmatch.str("sidestype").std::string::empty()) { // sidestype parsing if begins
        if (staticmatch.str("sidestype") == "\%") {             // if is a single % character.
          numSides = 100;
          if (g_arg_debug_flag) {
            std::cout << "numSides was % and is now:" << numSides << std::endl;
          }
        } else {
          numSides = std::stol(staticmatch.str("sidestype"));
          if (g_arg_debug_flag) {
            std::cout << "numSides is now:" << numSides << std::endl;
          }
        }
      } else {
        // sidestype is empty, setting numSides to a default of 6
        numSides = 6;
        if (g_arg_debug_flag) {
          std::cout << "numSides was empty and is now:" << numSides << std::endl;
        }
      } // sidestype parsing if ends
      // ready to pass parameters and throw the dice now ?
      if (g_arg_debug_flag) {
        std::cout << "ready to pass parameters and throw dice\n";
        std::cout << "calling and returning resultString = performDiceRoll(" << numThrows << "," << numDice << ","
                  << numSides << ")" << std::endl;
      }
      resultString = performDiceRoll(numThrows, numDice, numSides);
    } else {
      // @TODO improve bad dice string parse with a better error message
      std::cout << g_appname_string << ":"
                << " problem with a malformed dice string, (" << smatchFailedReasonString << ")" << std::endl;
      return "";
    }
  } catch (srell::regex_error &e) {
    // Syntax error in the regular expressioa
    std::cerr << "srell::regex_error occurred:" << e.what() << std::endl;
    // Crash / Return / Empty String
    return "";
  }
  // q = number of dice, s = sides, m=post modifier
  return resultString;
}

// std::string performDiceRoll(long numThrows, long numDice, long numSides) {
std::string performDiceRoll(long numThrows, long numDice, long numSides) {
  // if g_opt_s is true be verbose
  long throwCounter = 0;
  long rollCount = 0;
  long rollTotal = 0;
  // for easy unconversion of
  std::stringstream stringStream;
  std::string resultString = "";
  if (g_arg_debug_flag) {
    std::cout << "DEBUG: performDiceRoll(" << numThrows << "," << numDice << "," << numSides << ")\n";
  }
  // change to nested for loops do while is doing weird stuff
  for (int i = 0; i < numThrows; ++i) {
    throwCounter++; // increment throwCounter
    if (g_arg_debug_flag) {
      std::cout << "DEBUG: in the numThrows for loop afterthrowCounter postincrement::"
                << "performDiceRoll(" << numThrows << "," << numDice << "," << numSides << ")"
                << "\n"
                << "\tthrowCounter:" << throwCounter << " rollCount:" << rollCount << "throwTotal:" << rollTotal
                << std::endl;
    }
    if (i > 0) {
      // for all throws after the first append a carriage return or space
      if (g_arg_separate_flag) {
        stringStream << "\n";
      } else {
        stringStream << " ";
      }
    }
    if (g_arg_separate_flag) {
      stringStream << "Roll #" << ++rollCount << ": (";
    }
    for (int j = 0; j < numDice; j++) {
      int singleDieResult = rollDie(numSides);
      if (g_arg_debug_flag) {
        std::cout << "DEBUG: in the numDice for loop gathering ::singleDieResult::"
                  << "performDiceRoll(" << numThrows << "," << numDice << "," << numSides << ")"
                  << "\n"
                  << "\tthrowCounter:" << throwCounter << " rollCount:" << rollCount << " throwTotal:" << rollTotal
                  << " for_loop_counter j:" << j << "numDice:" << numDice << std::endl;
      }
      if (g_arg_separate_flag) {
        stringStream << singleDieResult;
      }
      rollTotal += singleDieResult;
      // numDice--;
      if (g_arg_separate_flag) {
        if ((numDice - 1) == j) {
          // no dice remain and separate_flag is true
          //  end the parenthesis
          stringStream << ")";
        } else { // adds a space after each single roll result except at end
          stringStream << " ";
        }
      }
    }
    // set up a string stream with which to
    // append die results to the result stream:
    if (g_arg_separate_flag) {
      stringStream << " = " << rollTotal;
    } else {
      stringStream << rollTotal;
    }
    if (g_arg_debug_flag) {
      std::cout << "between throws (here there be dragons?)" << std::endl;
    }
    // reset rollTotal between throws
    rollTotal = 0;
  } // for numThrows loop end
  // append stringStream to resultString then return resultString to caller
  resultString.append(stringStream.str());
  return resultString;
}

// returns a random integer
long rollDie(long sides) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(1, sides);

  int result = 0;
  result += dis(gen);
  if (g_arg_debug_flag) {
    std::cout << "in rollDie(" << sides << ") result: " << result << "\n";
  }
  return result;
}
