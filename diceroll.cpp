#include <iostream>
#include <string>
#include <sstream>
//#include <regex> //-- SRELL testing
// WHY no named capture groups in std::regex????? -- much sadness
#include <random>
#include <vector>

// args library for command line parsing https://github.com/taywee/args
#include "args.hxx"
// SRELL std regex like library by a kenotsuki https://akenotsuki.com
#include "srell.hpp"
// Global args with g_arg_ prefix
  bool g_arg_version_flag = false;
  bool g_arg_separate_flag = false;
  bool g_arg_interactive_flag = false;
// function declarations
std::string parseDiceString(std::string);

std::string performDiceRoll(long throws, long dice, long sides);

int rollDie(long sides);

int main(int argc, char *argv[]) {
  // help option
  // initially assume a single dice string
  // eventually become a vector of possibly multiple dicestrings
  std::string dice_string = "";


  // set up the argument parser and arguments
  args::ArgumentParser argsParser("diceroll program", "@TODO Epilogue.");
  args::HelpFlag help(argsParser, "help", "Display this help menu", {'h', "help"});
  args::Flag version(argsParser, "version", "@TODO Display the version diceroll",
                     {'v', "version"});
  args::Flag separate(argsParser, "separate",
                        "print out the Throw/Roll number and separate output of each die "
                        "separately as well as operations and totals",
                        {'s', "separate"});
  args::Flag interactive(argsParser, "interactive",
                         "@TODO interactive mode, diceroll terminal"
                         "interacts with the user in a command line way",
                         {'i', "interactive"});
  args::Group positionals(argsParser, "dice roll string");
  args::PositionalList<std::string> diceStringList(
      positionals, "dice strings", "parseable dice roll description strings");
  args::CompletionFlag completion(argsParser, {"complete"});
  try {
    argsParser.ParseCLI(argc, argv);
    for (auto &&diceString : diceStringList) {
      // for now echo entered dice strings
      std::cout << "running parseDiceString(" << diceString << ")\n";
      std::string result = parseDiceString(diceString);
      std::cout << "result:" << result << "\n";
    }
    // after parsing print an end line
  } catch (const args::Help &) {
    // print the pretty parsers autoconstructed help message then cleanly
    // exit -- no need for a help flag even, just default args:: behavior
    std::cout << argsParser;
    return 0;
  //} catch (const args::){} // g_arg_interactive_flag
  //} catch (const args::){} // g_arg_version_flag
  //} catch (const args::){} // g_arg_separate_flag
  } catch (const args::ParseError &e) {
    std::cerr << e.what() << std::endl;
    std::cerr << argsParser;
    return 1;
  }
  return 0;
}

// function definitions
std::string parseDiceString(std::string diceString) {
  std::string resultString = "";
  // compose a suitable rolldice compatible regex with named groups over
  // several lines
  std::string regexString =

      "^"
      "(?<throwgroup>(?<throws>([0-9]){1,})(?<throwch>[x|X]){1}){0,}"
      "(?<dicegroup>(?<numdice>[0-9]{1,})(?<dicechar>d|D){1}(?<sidestype>"
      "[0-9]{0,}|%{1})){0,}"
      "$";

  std::cout << "in parseDiceString(" << diceString << ")\n";
  try {
    //
    srell::regex expression(regexString);
    // static match
    srell::smatch staticmatch;
    if (srell::regex_search(diceString, staticmatch, expression)) {
      std::cout << "regex_search true: " << std::endl;
      std::cout << "m.size is:" << staticmatch.size() << std::endl;
      for (int i = 0; i < staticmatch.size(); i++) {
        std::cout << "m.str(" << i << "):" << staticmatch.str(i) << std::endl;
      }
      /* @TODO wrap with verbose arg */
      std::cout << "throwgroup: " << staticmatch.str("throwgroup") << " " << std::endl;
      std::cout << "\tthrows: " << staticmatch.str("throws") << " " << std::endl;
      std::cout << "\tthrowch: " << staticmatch.str("throwch") << " " << std::endl;
      std::cout << "dicegroup: " << staticmatch.str("dicegroup") << " " << std::endl;
      std::cout << "\tnumdice: " << staticmatch.str("numdice") << " " << std::endl;
      std::cout << "\tdicechar: " << staticmatch.str("dicechar") << " " << std::endl;
      std::cout << "\tsidestype: " << staticmatch.str("sidestype") << " " << std::endl;

      // initialize throw dice and sides values to sensible defaults if not parsed
      long numThrows = 1; long numDice = 1; long numSides = 6;

      std::cout << "c_str run on throws becomes:"
                << staticmatch.str("throws").std::string::c_str() << std::endl;
      std::cout << std::boolalpha; // turn on printing boolean true/false rather
                                   // than 1/0
      std::cout << "empty test on throws:"
                << staticmatch.str("throws").std::string::empty() << std::endl;
      std::cout << std::noboolalpha; // turn off printing boolean true/false
                                     // rather than 1/0

      if (!staticmatch.str("throws").std::string::empty()) {
        std::cout << "throws is not empty" << std::endl;
        numThrows = std::stol(staticmatch.str("throws"));
        std::cout << "numThrows is now:" << numThrows << std::endl;
      }
      if (!staticmatch.str("numdice").std::string::empty()) {
        std::cout << "numdice is not empty" << std::endl;
        numDice = std::stol(staticmatch.str("numdice"));
        std::cout << "numDice is now:" << numDice << std::endl;
      }
      if (!staticmatch.str("sidestype").std::string::empty()) {
        std::cout << "sidestype is not empty" << std::endl;
        // not empty and is not a literal single % character.
        if (staticmatch.str("sidestype") == "\%") {
          std::cout << "sidestype is a fancy % character" << std::endl;
          numSides = 100;
          std::cout << "numSides was a fancy % and is now:" << numSides
                    << std::endl;
        } else {
          numSides = std::stol(staticmatch.str("sidestype"));
        }
        std::cout << "numSides is now:" << numSides << std::endl;
      }
      // do/While
	    std::cout << "numThrows:" << numThrows << std::endl;
      // std::cout << "matchstr: " << m.str("matchstr") << " " <<
      // std::endl;
      // if ( m.str("throwgroup") ) {
      // std::cout << "m.size() " << m.size << std::endl;
      // std::cout << "m.match_results.size= " << m.match_results();
      //}else{
      //}
    } else {
      std::cout << "regex_search false" << std::endl;
      return "";
    }
  } catch (srell::regex_error &e) {
    // Syntax error in the regular expressioa
    std::cerr << "srell::regex_error occurred:" << e.what() << std::endl;
    // Crash / Return /
    return "";
  }
  // q = number of dice, s = sides, m=post modifier
  return resultString;
}
std::string performDiceRoll(long throws, long dice, long sides ) {
      // if g_opt_s is true
      std::string resultStringStream = "";
      do {
        // per throw (at least once)
        do {
          int singleDieResult = rollDie(sides);

          resultString.append("");
          numDice--;
        } while (numDice);
        // set up a string stream with which to 
        // append die results 
        resultStringStream << "";
        // decrement throw
        numThrows--;
      } while (numThrows);
  return "";
}
int rollDie(long sides) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(1, sides);

  int result = 0;
  // for (int i = 0; i < quantity; i++) {
  result += dis(gen);
  //}

  return result;
}
