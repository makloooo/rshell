# CS100 Project 1 - rshell

## What it is

rshell is supposed to be a written imitation of GNU's bash. It should be able to support most if not all of bash's non-built in commands.

## What it does

This shell can do the following:
* Run multiple commands with '||', '&&', and ';' logic.
* Run commands with comment support. 
* Checks for dangling connectors
    * If you end with a connector which requires another argument, you will be prompted for further input.
    * This will continue until you end with an argument
* Grab your current login and hostname information
* Supports end-attached ';' syntax.

## What it does wrong

This shell has multiple problems:
* If you try to echo a phrase in quotes, it will include the quotation marks as well.
* If you attach a double connector ('||' or '&&') to the beginning of an argument, it will be read in as a separate argument.
* If you attach any sort of connector to the beginning or end of an argument, it will be read in as part of the argument.
