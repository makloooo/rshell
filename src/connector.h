#ifndef CONNECTOR_H
#define CONNECTOR_H

#include "main.h"

class connector {
    protected:
        char* cmd1[256]; // Holds commands
        char* cmd2[256];
        
        bool execute(char* args[]) {

            int status;
            pid_t c_pid, pid; // child_processID, processID
            c_pid = fork(); 
            // Forks our process and stores its ID into a variable "child id"

            if (strcmp(args[0], "exit") == 0) exit(0);

            // First fork a process.
            if (c_pid < 0) { 
                // If the fork doesn't take place at all => It fails
                perror("fork failed");
                exit(EXIT_FAILURE);
            }
            else if (c_pid == 0) { 
                // If the fork succeeds, and this is the child process
                // cout << "[DEBUG] --------------------" << endl;
                // cout << "[DEBUG] I'm a child process." << endl;
                // cout << "[DEBUG] Executing "; printArgs(args); cout << endl;
                execvp(args[0], args);
                perror("execvp failed");
                exit(EXIT_FAILURE);
            }
            else if (c_pid > 0) { 
                // If the fork succeeds, and this is the parent process
                // cout << "[DEBUG] I'm the parent process." << endl;
                if ( (pid = wait(&status)) < 0) {
                    perror("wait failed");
                    exit(EXIT_FAILURE);
                }
                // cout << "[DEBUG] -------------------" << endl;
                // cout << "[DEBUG] Child finished its job with exit status " 
                //      << status << '.' << endl;
            }
            // cout << "[DEBUG] WIFEXITED yielded "; 
            // (WIFEXITED(status)) ? cout << "true" : cout << "false"; 
            // cout << endl;
            return (status == 0); // I'M CHEATING
        }

    public:
        virtual bool run() = 0;
        virtual bool run(bool exitStatus) = 0;
};

class Ampersand : public connector {
    public:
        Ampersand(char* first[], char* second[] = '\0') {
            for (int i = 0; first[i] != '\0'; ++i) cmd1[i] = first[i];
            if (second != '\0') { 
                for (int i = 0; second[i] != '\0'; ++i) cmd2[i] = second[i];
            }
        }
        ~Ampersand() {
            setNull(cmd1); // NOT VALUES, JUST POINTERS
            setNull(cmd2);
        }
        bool run() {
            // cout << "[DEBUG] Running an exit independent execution!" << endl;
            if (!execute(cmd1)) return false; // And don't run the next command
            // cout << "[DEBUG] Running second command." << endl;
            return execute(cmd2); // If success, run the next command
        }
        bool run(bool exitStatus) { 
            // For subsequent runs, you're only going to hold one command;
            // cout << "[DEBUG] Running an exit dependent execution!" << endl;
            if (exitStatus) { // If the last command ran correctly
                return execute(cmd1);
            }
            return false; // Otherwise don't run it, and just return.
        }
};

class DoubleBars : public connector {
    public:
        DoubleBars(char* first[], char* second[] = '\0') {
            for (int i = 0; first[i] != '\0'; ++i) cmd1[i] = first[i];
            if (second != '\0') { 
                for (int i = 0; second[i] != '\0'; ++i) cmd2[i] = second[i];
            }
        }
        ~DoubleBars() {
            setNull(cmd1);
            setNull(cmd2);
        }
        bool run() {
            // cout << "[DEBUG] Running an exit independent execution!" << endl;
            if (!execute(cmd1)) {
                return execute(cmd2); // And run the next command
            }
            return true; 
            // If cmd1 executed correctly:
            // The if statement should not occur and just return true.
        }        
        bool run(bool exitStatus) { 
            // For subsequent runs, you're only going to hold one command;
            // cout << "[DEBUG] Running an exit dependent execution!" << endl;
            if (!exitStatus) { // If the last command didn't run correctly
                return execute(cmd1); // I mean, if this doesn't run correctly, then return false for all.
            }
            return true; // Otherwise don't run it, and just return.
        }
};

class Semicolon : public connector {
    public:
        Semicolon(char* first[]) {
            for (int i = 0; first[i] != NULL; ++i) {
                cmd1[i] = first[i];
                cmd1[i+1] = '\0';
            }

        }
        ~Semicolon() {
            setNull(cmd1);
        }
        bool run() {
            execute(cmd1);
            // If failed, run the next command anyway
            return true; // So it should always return true
        }
        bool run(bool exitStatus) {
            return run(); // Not defining the function scares me.
        }
};

#endif
