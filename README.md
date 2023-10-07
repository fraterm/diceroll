# diceroll
![Diceroll Logo](./Diceroll_Logo.svg)

## A C++ Command Line Dice Rolling Program

* uses the args argument parser as a CLI parser API https://taywee.github.io/args
* using the https://github.com/ZimProjects srell std::regex-like librarylibrary to parse the dice input strings https://www.akenotsuki.com/misc/srell/
* initially aiming at feature parity with the wonderful rolldice https://github.com/sstrickl/rolldice


## DICE STRING FORMAT

* The dice string uses the following format by default (a subset of the rolldice dice string format)

```
{#x}{#}d[#|%]
```

which becomes an ECMAScript (JavaScript) regex

currently translated to a an srell::regex string with posix-classes and named-groups thusly:

```
	std::string regexString = 
			"^" // beginning of string anchor
			"(?<throwgroup>(?<throws>([0-9]){1,})(?<throwch>[x|X]){1}){0,}"  // throwgroup
			"(?<dicegroup>(?<numdice>\\d{1,})(?<dicechar>d|D){1}(?<numsidesortype>\\d{0,}|%{1})){0,}"  // dicegroup
			"$";  // end of string anchor
```


The format that rolldice C program uses:

```
{#x}{#}d[#|%]{*#}{+/-#}{s#}
```

Stage the string parse into parts:
  Number of throws part optional
  Number and type of dice part required

## Future Plans 
* If there's no string, drop into a "rolling shell" or "rolling session" where you type dicestrings and they are parsed from the line... 
* fill in the options existing to operate similarly to rolldice
* someday I might integrate with GNOLLs amazing TTRPG dice string parser.
* Profit?
