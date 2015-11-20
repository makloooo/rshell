#ifndef HELPER_H
#define HELPER_H

#include "include.h"
#include "connector.h"

class Connector;

void truncate(char*&);
void trim(char*&);
list<char*> parse(char*);
void printArg(char*);
void printArgs(char**);
void printInfo(char*, char*);
void printQueue(list<char**>);
void printQueue(list<char*>);
void printLine(int);
void setNull(char*);
void cutEndSpaces(char*);
bool test(char**);
Connector* buildTree(list<char**>&, list<char*>&);
void printTree(Connector*);

int checkConnectors(char*);
bool isConnector(char*);
bool isDblConnector(char*);
bool isAttached(char*);
bool hasStartParenthesis(char*&);
bool hasEndParenthesis(char**&);
#endif
