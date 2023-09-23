# diceroll

## A C++ Command Line Dice Rolling Utility

* uses the args argument parser as a CLI parser API https://taywee.github.io/args
* options will likely be similar to rolldice https://github.com/sstrickl/rolldice
* currently using std::regex to parse the dicestrings to generate the throws

## DICE STRING FORMATS
* The dice string uses the following format by default but will support different dice string formats:
```
{#x}{#}d[#|%]{*#}{+/-#}{s#}
```
