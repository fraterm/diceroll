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
// function declarations
// parses Dice Strings
std::string parseDiceString(std::string);
// performs Dice Throws
std::string performDiceRoll(long numThrows, long numDice, long numSides, long numMult, char chAddSub, long numAddSub,
                            long numDropLow);
// generates random long integers from 1 to sides
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
  long numMult = 1;
  char chAddSub = '\0'; // default to null character
  long numAddSub = 0;
  long numDropLow = 0;
  std::string resultString = "";
  std::string smatchFailedReasonString = "unknown"; // @TODO
  // compose a suitable rolldice compatible regex with named groups over
  // several lines
  std::string regexString = "^"
                            "(?<throwgroup>(?<throws>([0-9]){1,})(?<throwch>[x|X]){1}){0,}"
                            "(?<dicegroup>(?<numdice>[0-9]{1,})(?<dicechar>[d|D]){1}(?<sidestype>[0-9]{0,}|[%]{1})){0,}"
                            "(?<multgroup>(?<charmult>[*]){1}(?<nummult>[0-9]){1,}){0,}"
                            "(?<addsubgroup>(?<chaddsub>[+|-]){1}(?<numaddsub>[0-9]){1,}){0,}"
                            "(?<droplowgroup>(?<chardroplow>[s]){1}(?<numdroplow>[0-9]){1,}){0,}"
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
        std::cout << "\tcharmult: " << staticmatch.str("charmult") << " " << std::endl;
        std::cout << "\tnummult: " << staticmatch.str("nummult") << " " << std::endl;
        std::cout << "addsubgroup: " << staticmatch.str("addsubgroup") << " " << std::endl;
        std::cout << "\tchaddsub: " << staticmatch.str("chaddsub") << " " << std::endl;
        std::cout << "\tnumaddsub: " << staticmatch.str("numaddsub") << " " << std::endl;
        std::cout << "droplowgroup: " << staticmatch.str("droplowgroup") << " " << std::endl;
        std::cout << "\tchardroplow: " << staticmatch.str("chardroplow") << " " << std::endl;
        std::cout << "\tnumdroplow: " << staticmatch.str("numdroplow") << " " << std::endl;

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
      // nummult isn't empty?
      if (!staticmatch.str("nummult").std::string::empty()) { // nummult parsing if begins
        numMult = std::stol(staticmatch.str("nummult"));
        if (g_arg_debug_flag) {
          std::cout << "numMult is now:" << numMult << std::endl;
        }
      }
      if (!staticmatch.str("chaddsub").std::string::empty()) {
        // convert string to c_str
        chAddSub = (staticmatch.str("chaddsub").c_str()[0]);
        if (g_arg_debug_flag) {
          std::cout << "chAddSub is now:" << chAddSub << std::endl;
        }
      }
      if (!staticmatch.str("numaddsub").std::string::empty()) {
        numAddSub = std::stol(staticmatch.str("numaddsub"));
        if (g_arg_debug_flag) {
          std::cout << "staticmatch.str(\"numaddsub\") is not empty" << std::endl;
          std::cout << "numAddSub is now:" << numAddSub << std::endl;
        }
      }
      if (!staticmatch.str("numdroplow").std::string::empty()) {
        numDropLow = std::stol(staticmatch.str("numdroplow"));
        if (g_arg_debug_flag) {
          std::cout << "staticmatch.str(\"numdroplow\") is not empty" << std::endl;
          std::cout << "numDropLow is now:" << numDropLow << std::endl;
        }
      }
      // ready to pass parameters and throw the dice now ?
      if (g_arg_debug_flag) {
        std::cout << "ready to pass parameters and throw dice\n";
        std::cout << "calling and returning resultString = performDiceRoll(" << numThrows << "," << numDice << ","
                  << numSides << ")" << std::endl;
      }
      resultString = performDiceRoll(numThrows, numDice, numSides, numMult, chAddSub, numAddSub, numDropLow);
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

std::string performDiceRoll(long numThrows, long numDice, long numSides, long numMult, char chAddSub, long numAddSub,
                            long numDropLow) {
  // if g_opt_s is true be verbose
  long throwCounter = 0;
  long rollCount = 0;
  long rollTotal = 0;
  // stream to load with << for eventual return at end
  // collection of rolls
  std::vector<long> rollVector;
  // so that lowest n can be dropped depending on args
  std::vector<long> dropVector;
  std::stringstream stringStream;
  std::string resultString = "";
  if (g_arg_debug_flag) {
    std::cout << "DEBUG: performDiceRoll(" << numThrows << "," << numDice << "," << numSides << "," << numMult << ","
              << chAddSub << "," << numAddSub << "," << numDropLow << ")" << std::endl;
  }
  // for every numThrows
  for (int i = 0; i < numThrows; ++i) {
    throwCounter++; // increment throwCounter
    if (g_arg_debug_flag) {
      std::cout << "beginning of for every numThrows\n"
                << "\t"
                << "throwCounter:" << throwCounter << " rollCount:" << rollCount << " throwTotal:" << rollTotal
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
        std::cout << " DEBUG: "
                  << " for_loop_counter j: " << j << " numDice: " << numDice << " throwCounter: " << throwCounter
                  << " rollCount: " << rollCount << " rollTotal: " << rollTotal << std::endl;
      }
      if (g_arg_separate_flag) {
        stringStream << singleDieResult;
      }
      // add to rollTotal in the simple case
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
      if (numMult > 1) {
        stringStream << " * " << numMult;
      }
      // if chAddSub +
      if (chAddSub == '+') {
        //
        stringStream << " + " << numAddSub;
      }
      // if chAddSub -
      if (chAddSub == '-') {
        stringStream << " - " << numAddSub;
      }
      // apply multiplication to total here first then apply addition or subtraction to total
      if (numMult > 1) {
        rollTotal *= numMult;
        if (g_arg_debug_flag) {
          std::cout << "rollTotal: " << rollTotal << " numMult: " << numMult << std::endl;
        }
      }
      // if chAddSub +
      if (chAddSub == '+') {
        rollTotal += numAddSub;
      }
      if (chAddSub == '-') {
        rollTotal -= numAddSub;
      }
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
