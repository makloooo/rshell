# CS100 Project - rshell

## What it is

rshell is supposed to be a written imitation of GNU's bash. It should be able to support most if not all of bash's non-built in commands. It's built around a very loose and dirty composite pattern.

## What it does

This shell can do the following:
* Run multiple commands with '||', '&&', and ';' logic.
* Run commands with comment support. 
* Checks for dangling connectors
    * If you end with a connector which requires another argument, you will be prompted for further input.
    * This will continue until you end with an argument
* Grab your current login and hostname information
* Supports end-attached ';' syntax.
* Supports precedence using parentheses around operands.
* Supports the use of the 'test' command with only -e, -f, and -d flags.

## What it does wrong

This shell has multiple problems:
* If you attach any sort of double connector ('||', '&&') to the beginning or end of an argument, it will be read in as part of the argument.
* This is not idiot proof, so if you forgot to terminate a quote or parenthesis, you will most likely get an infinite loop or segmentation fault.
