#include <iostream>
#include <string>
//#include <regex> //-- SRELL testing
// WHY no named capture groups in std::regex????? -- much sadness
#include <random>
#include <vector>

// args library for command line parsing https://github.com/taywee/args
#include "args.hxx"
// SRELL std regex like library by a kenotsuki https://akenotsuki.com
#include "srell.hpp"
// function declarations
std::string parseDiceString(std::string);

int rollDice(long sides, long quantity);

int main(int argc, char *argv[]) {
  // help option
  // initially assume a single dice string
  // eventually become a vector of possibly multiple dicestrings
  std::string dice_string = "";

  bool version_bool = false;
  bool exhaustive_bool = false;
  bool interactive_bool = false;
  bool help_bool = false;

  // set up the argument parser and arguments
  args::ArgumentParser parser("diceroll program", "@TODO Epilogue.");
  args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
  args::Flag version(parser, "version", "@TODO Display the version diceroll",
                     {'v', "version"});
  args::Flag exhaustive(parser, "exhaustive",
                        "@TODO Set the Exhaustive output of each die "
                        "separately as well as operations and totals",
                        {'e', "exhaustive"});
  args::Flag interactive(parser, "interactive",
                         "@TODO Set the Exhaustive output of each die "
                         "separately as well as operations and totals",
                         {'i', "interactive"});
  args::Group positionals(parser, "dice roll string");
  args::PositionalList<std::string> diceStringList(
      positionals, "dice strings", "parseable dice roll description strings");
  args::CompletionFlag completion(parser, {"complete"});
  try {
    parser.ParseCLI(argc, argv);
    for (auto &&diceString : diceStringList) {
      // for now echo entered dice strings
      std::cout << "running parseDiceString(" << diceString << ")\n";
      std::string result = parseDiceString(diceString);
      std::cout << "result:" << result << "\n";
    }
    // after parsing print an end line
  } catch (const args::Help &) {
    // print the pretty parsers autoconstructed help message then cleanly
    // exit --
    std::cout << parser;
    return 0;
  } catch (const args::ParseError &e) {
    std::cerr << e.what() << std::endl;
    std::cerr << parser;
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
      "(?<throwgroup>(?<throws>(\\d){1,})(?<throwch>[x|X]){1}){0,}"
      "(?<dicegroup>(?<numdice>\\d{1,})(?<dicechar>d|D){1}(?<sidestype>"
      "\\d{0,}|%{1})){0,}"
      "$";

  //"^"
  //"(?<throwgroup>(?<numthrows>[[:digit:]]{1,})([x|X]{1}){0,1})"
  //"(?<dicegroup>(?<numdice>[[:digit:]])(?<dicechar>[d|D]{1})(?<numsides>[[:digit:]{0,}|%{1}])){0,1}"
  //"(?<endgroup>.{0,})"
  //"$";
  std::cout << "in parseDiceString(" << diceString << ")\n";
  try {
    //
    srell::regex e(regexString);
    // static match
    srell::smatch m;
    if (srell::regex_search(diceString, m, e)) {
      std::cout << "regex_search true: " << std::endl;
      std::cout << "m.size is:" << m.size() << std::endl;
      for (int i = 0; i < m.size(); i++) {
        std::cout << "m.str(" << i << "):" << m.str(i) << std::endl;
      }
      // std::cout << "m.str(0):" << m.str(0) << std::endl;
      // std::cout << "m.str(1):" << m.str(1) << std::endl;
      // std::cout << "m.str(2):" << m.str(2) << std::endl;
      // std::cout << "m.str(3):" << m.str(3) << std::endl;
      // std::cout << "m.str(4):" << m.str(4) << std::endl;
      // std::cout << "m.str(5):" << m.str(5) << std::endl;
      // std::cout << "m.str(6):" << m.str(6) << std::endl;
      // std::cout << "m.str(7):" << m.str(7) << std::endl;
      std::cout << "throwgroup: " << m.str("throwgroup") << " " << std::endl;
      std::cout << "\tthrows: " << m.str("throws") << " " << std::endl;
      std::cout << "\tthrowch: " << m.str("throwch") << " " << std::endl;
      std::cout << "dicegroup: " << m.str("dicegroup") << " " << std::endl;
      std::cout << "\tnumdice: " << m.str("numdice") << " " << std::endl;
      std::cout << "\tdicechar: " << m.str("dicechar") << " " << std::endl;
      std::cout << "\tsidestype: " << m.str("sidestype") << " " << std::endl;
      long numThrows = 1;
      long numDice = 1;
      long numSides = 6;

      std::cout << "c_str run on throws becomes:" << m.str("throws").std::string::c_str() << std::endl;
      std::cout << std::boolalpha; // turn on printing boolean true/false rather than 1/0
      std::cout << "empty test on throws:" << m.str("throws").std::string::empty() << std::endl;
      std::cout << std::noboolalpha; // turn off printing boolean true/false rather than 1/0

      if (!m.str("throws").std::string::empty()){
	  numThrows = std::stol(m.str("throws"));
      }

      if (!m.str("numdice").std::string::empty()){
	  numDice = std::stol(m.str("numdice"));
      }

      if (!m.str("sidestype").std::string::empty()){
	  // not empty and is not a literal single % character.

	  if (m.str("sidestype")== "%"){
	      numSides = 100;
	  }else{
	      numSides = std::stol(m.str("throws").std::string::c_str());
	  }
      }



      // std::cout << "matchstr: " << m.str("matchstr") << " " <<
      // std::endl;
      // if ( m.str("throwgroup") ) {
      // std::cout << "m.size() " << m.size << std::endl;
      // std::cout << "m.match_results.size= " << m.match_results();
      //}else{
      //}
    } else {
      std::cout << "regex_search false" << std::endl;
    }
  } catch (srell::regex_error &e) {
    // Syntax error in the regular expression
    std::cerr << "srell::regex_error occurred:" << e.what() << std::endl;
    // Crash / Return /
    return "";
  }
  // q = number of dice, s = sides, m=post modifier
  return resultString;
}

int rollDice(long sides, long quantity) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(1, sides);

  int result = 0;
  for (int i = 0; i < quantity; i++) {
    result += dis(gen);
  }

  return result;
}
