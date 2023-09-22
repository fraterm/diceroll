#include <iostream>
#include <string>
#include <regex>
#include <random>
#include <vector>

// args library for command line parsing https://github.com/taywee/args 
#include "args.hxx"
// function declarations
std::string rollDiceString(std::string);
int roll_dice(int sides, int quantity);

int main(int argc, char *argv[])
{
	// help option
	// initially assume a single dice string
	// eventually become a vector of possibly multiple dicestrings
	std::string dice_string = "";
	bool help_bool = false;

	args::ArgumentParser parser("diceroll program", "This goes after the options.");
	args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
	args::Group positionals(parser, "positionals" );
	args::PositionalList<std::string> diceStringList(positionals, "dice strings", "parseable dice roll description strings");
	args::CompletionFlag completion(parser, {"complete"});
	try
	{
		parser.ParseCLI(argc, argv);
		for (auto &&diceString : diceStringList)
		{
			// for now echo entered dice strings
			std::cout << "running rollDiceString(" << diceString << ")\n";
			std::string result = rollDiceString(diceString);
			std::cout << "result:" << result << "\n";
			
			
		}
		// after parsing print an end line
	}
	catch (const args::Help&)
	{
		// print the pretty parsers autoconstructed help message then cleanly exit --
		std::cout << parser;
		return 0;
	}
	catch (const args::ParseError& e)
	{
		std::cerr << e.what() << std::endl;
		std::cerr << parser;
		return 1;
	}
	return 0;
}

// function definitions
std::string rollDiceString(std::string diceString){
	std::string resultString = "nothing yet";
	// validate and parse the dice string to generate random numbers properly
	// basic_regex matching 1D6 or 1D20 or considering 1D to be a default D6 or 1d
	// to be digested and resolved accordingly
	std::cout << "in rollDiceString(" << diceString << ")\n";
	// variables important for the simple match expressions 
	long numthrows; long quantity; long sides;
	// should match  1d6, 2D6, 15D5, D6
	try {
		// default to the c program rolldice dicestring to start
		std::regex diceExpr("^(\\d*)[x|X]*(\\d*)[d|D](\\d*)$" );
		std::smatch match;
		if (std::regex_search(diceString, match, diceExpr) && match.size() >= 1) {
			std::cout << "regex matched" << std::endl;
			std::cout << "numthrows: " << match.str(1) << " "
				<< "quantity: " << match.str(2) << " "
				<< "sides:" << match.str(3) << std::endl;
			numthrows = std::stol (match.str(1));
			quantity = std::stol (match.str(2));
			sides = std::stol (match.str(3));

			std::cout << "roll_dice("<< sides <<","<< quantity << ")" << roll_dice(sides, quantity) << std::endl;
		}else{
			std::cout << "regex didn't match" << std::endl;
		}
	} catch (std::regex_error& e) {
		// Syntax error in the regular expression
		std::cerr << "std::regex_error occurred:" << e.what() << std::endl;
		// Crash / Return / 
		return "";
	}
	// q = number of dice, s = sides, m=post modifier
	return resultString;	
}

int roll_dice(int sides, int quantity) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(1, sides);

  int result = 0;
  for (int i = 0; i < quantity; i++) {
    result += dis(gen);
  }

  return result;
}
