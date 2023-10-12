# diceroll
![Diceroll Logo](./Diceroll_Logo.svg)

## A C++ Command Line Dice Rolling Program

* uses the args argument parser as a CLI parser API https://taywee.github.io/args
* using the https://github.com/ZimProjects srell std::regex-like librarylibrary to parse the dice input strings https://www.akenotsuki.com/misc/srell/
* initially aiming at feature parity with the wonderful rolldice https://github.com/sstrickl/rolldice


## DICE STRING FORMAT

Initially uses the same format that the rolldice C program uses:

```
{#x}{#}d[#|%]{*#}{+/-#}{s#}
```

### number of rolls or throws of the dice group specified later, optional.
```
{#x}
```

### number of dice then their sides, or the % to represent 100 beginning required
```
{#}d[#|%]
```

Which becomes an ECMAScript (JavaScript) regex with named-groups parseable by srell regex library:  

```
    std::string regexString = "^"
                            "(?<throwgroup>(?<throws>([0-9]){1,})(?<throwch>[x|X]){1}){0,}"
                            "(?<dicegroup>(?<numdice>[0-9]{1,})(?<dicechar>[d|D]){1}(?<sidestype>[0-9]{0,}|[%]{1})){0,}"
                            "(?<multgroup>(?<multchar>[*]){1}(?<multnum>[0-9]){1,}){0,}"
                            "(?<addsubgroup>(?<addsubchar>[+|-]){1}(?<addsubnum>[0-9]){1,}){0,}"
                            "(?<droplowestgroup>(?<droplowestchar>[s]){1}(?<droplowestnum>[0-9]){1,}){0,}"
                            "$";
```



Stage the string parse into parts:
  Number of throws part optional
  Number and type of dice part required

## Future Plans 
* If there are no positional string arguments, drop into a "rolling shell" or "rolling session" where you type dicestrings and they are parsed from the line... 
* fill in the options existing to operate similarly to rolldice
* someday I might integrate with GNOLLs amazing TTRPG dice string parser.
* Profit?
