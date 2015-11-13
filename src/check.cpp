#include "check.h"

int checkConnectors(char* argv) {
    // This will be returning flags.
    if (strcmp(argv, ";") == 0 || argv[strlen(argv) - 1] == ';') return 0;
    else if (strcmp(argv, "#") == 0) return 1;
    else if (strcmp(argv, "||") == 0) return 2;
    else if (strcmp(argv, "&&") == 0) return 3;
    else return -1;
}

bool isConnector(char* argv) {
    if (checkConnectors(argv) >= 0) return true;
    return false;
}

bool isDblConnector(char* argv) {
    if (checkConnectors(argv) > 1) return true;
    return false;
}

bool isAttached(char* argv) {
    if (strlen(argv) > 1 && (argv[strlen(argv) - 1] == ';')) return true;
    return false;
}

