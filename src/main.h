#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <queue>
#include <deque>

using namespace std;

int checkConnectors(char* argv) {
    // This will be returning flags.
    if (argv[strlen(argv) - 1] == ';') return 0;
    else if (strcmp(argv, "||") == 0) return 1;
    else if (strcmp(argv, "&&") == 0) return 2;
    else if (strcmp(argv, "#") == 0) return 3;
    else return -1;
}

bool isConnector(char* argv) {
    if (checkConnectors(argv) >= 0) return true;
    return false;
}

bool isDblConnector(char* argv) {
    if (checkConnectors(argv)> 0) return true;
    return false;
}

void truncate(char* argv) {
    argv[strlen(argv) - 1] = '\0';
    return;
}

void printArg(char* argv) {
    for (int i = 0; argv[i] != '\0'; ++i) {
        cout << argv[i];
    }
    return;
}

void printArgs(char* argv[]) {
   for (int i = 0; argv[i] != '\0'; ++i) {
        printArg(argv[i]);
        if (argv[i+1] != '\0') cout << ' ';
    }
}

void printQueue(queue<char**> q) {
    for (int i = 0; !q.empty(); ++i) {
        cout << "[DEBUG] Argument #" << i << ": ";
        printArgs(q.front());
        q.pop();
        cout << endl;
    }
    return;
}

void printQueue(deque<char*> q) {
    while (!q.empty()) {
        cout << q.front() << ' ';
        q.pop_front();
    }
    cout << endl;
    return;
}

void printQueue(queue<char*> q) {
    while (!q.empty()) {
        cout << q.front() << ' ';
        q.pop();
    }
    cout << endl;
    return;
}

void cleanArg(char cTokens[]) {
    for (int i = 0; cTokens[i] != '\0'; ++i) {
        delete (cTokens + i);
    }
    return;
}

void cleanArgs(char* cTokens[]) {
    for (int i = 0; cTokens[i] != '\0'; ++i) {
        cleanArg(cTokens[i]);
    }
    return;
}

void setNull(char* cTokens[]) {
    for (int i = 0; cTokens[i] != '\0'; ++i) {
        for (int n = 0; cTokens[n] != '\0'; ++n) {
            cTokens[i][n] = '\0'; // Handling dangling pointers
        }
        cTokens[i] = '\0';
    }
    return;
}

void cutEndSpaces(char* cstr) {
    for (int i = 0; cstr[i] != '\0'; ++i) {
        if (cstr[i] == ' ') cstr[i] = '\0';
    }
    return;
}

bool execute(char* args[]) {

    int status;
    pid_t c_pid, pid; // child_processID, processID
    c_pid = fork(); // Forks our process and stores its ID into a variable "child id"

    // First fork a process.
    if (c_pid < 0) { // If the fork doesn't take place at all => It fails
        perror("Fork failed");
        exit(1);
    }
    else if (c_pid == 0) { // If the fork succeeds, and this is the child process
        // cout << "[DEBUG] --------------------" << endl;
        // cout << "[DEBUG] I'm a child process." << endl;
        // cout << "[DEBUG] Executing '"; printArgs(args); cout << "'!" << endl;
        // Once execvp is called, the program is overwritten, and this function never finishes. 
        // That's why it's okay to leave that hanging there. 
        execvp(args[0], args);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }
    else if (c_pid > 0) { // If the fork succeeds, and this is the parent process
        // cout << "[DEBUG] I'm the parent process." << endl;
        if ( (pid = wait(&status)) < 0) {
            perror("Something went wrong while waiting");
            exit(1);
        } // Parent infinitely loops while the child carries out its duty.
        // cout << "[DEBUG] --------------------" << endl;
        // cout << "[DEBUG] Child finished its job. Whew!" << endl;
    }
    // This should only be run if the command is valid.
    return (status == 0);
}

#endif
