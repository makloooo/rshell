#ifndef HELPER_H
#define HELPER_H

#include "include.h"
#include "connector.h"

void truncate(char* argv);
deque<char*> parse(char cstrcmd[]);
void printArg(char* argv);
void printArgs(char* argv[]);
void printInfo(char* login, char host[]);
void printQueue(deque<char**> q);
void printQueue(deque<char*> q);
void printQueue(queue<char*> q);
void setNull(char* cTokens[]);
void cutEndSpaces(char* cstr);
bool execute(char* args[]);
void runCommands(queue<char**> arguments, queue<char*> connectors);

#endif
