#include "connector.h"
#include "main.h"

int main(int argc, char* argv[], char *envp[]) {
   
    // If you use execvp() without a fork, it'll take over the main system.
    // This is why you need to fork a process, then call execvp()

    queue<char**> arguments;
    queue<char*> connectors;

    // Grabs host name
    char* login;
    char host[127];

    login = getlogin();
    if (gethostname(host, 127) != 0) {
        perror("failed to gethostname");
        exit(EXIT_FAILURE);
    }
   
    string command = "";

    printInfo(login, host);
    cout << "$ ";
    while (getline(cin, command)) {

        while (command.length() == 0) {
            printInfo(login, host);
            cout << "$ ";
            getline(cin,command);
        }

        char* cstrcmd = '\0';
        deque<char*> tokens;
        queue<char*> data;

        cstrcmd = new char[command.size() + 1]; // This data is universal. It will be used until deallocated.

        strcpy(cstrcmd, command.c_str());
        data.push(cstrcmd); // This is to hold locations of the data to deallocate later
        tokens = parse(cstrcmd);


        /*if (isDblConnector(tokens.back())) {
            tokens.pop_back();
            cout << "Terminated without command -- ignoring leading connector." << endl;
        }*/

        while (isDblConnector(tokens.back())) { // If the stupid user ended with a connector
            deque<char*> tmpCmds;

            cout << "> ";
            getline(cin, command);

            cstrcmd = new char[command.size() + 1];
            strcpy(cstrcmd, command.c_str());
            data.push(cstrcmd);

            tmpCmds = parse(cstrcmd);
            while (!tmpCmds.empty()) {
                tokens.push_back(tmpCmds.front());
                tmpCmds.pop_front();
            }
        }

        int last; // There definitely has to be a better way to do this...
        char* cmd[128][256]; // cmd is a list of pointers to the beginning of each token of the universal data
        int n = 0;
        for (int i = 0; !tokens.empty(); ++i) {
            cutEndSpaces(tokens.front());
            if (isConnector(tokens.front())) {
                if (isAttached(tokens.front())) { // Pesky semicolons being attached...
                    truncate(tokens.front()); // e.g. -a; >> -a
                    cmd[n][i] = tokens.front(); // cmd[i] gets -a, contains pointer to the first character of that value, '-'
                    tokens.front() = new char[1]; // tokens.front() gets a pointer to new cString
                    data.push(tokens.front());
                    strcpy(tokens.front(), ";\0"); // strcpy sets its values to be ";\0"
                    ++i; // Progressing to the next value of cmd, since it was assigned in this instance
                }
                cmd[n][i] = '\0'; // Null terminating cmd
                arguments.push(cmd[n]); // If we ran into a connector, that means we have a full argument already
                // Arguments gets a pointer to a block of cstrings
                connectors.push(tokens.front());
              
                ++n;
                i = -1; // Reset argc
            }
            else {
                cmd[n][i] = tokens.front(); // Assigns cmd[i] to pointer at tokens.front(). Tokens contains cstrings
            }
            tokens.pop_front();
            last = i;
        }
        if (cmd[n][0] != '\0') { // Push in the last argument
            cmd[n][last+1] = '\0';
            arguments.push(cmd[n]);
        }

        // Runs through both queues simutaneously
        // After the first run, logic is checked parallel to arguments run
        bool exitStatus;
        bool exitIndep = true; // There's probably a way better way to do this...
        char** tmpArg;
        while (!arguments.empty()) {
            if (!connectors.empty()) {
                if (strcmp(connectors.front(), ";") == 0) {
                    if (!exitIndep) {
                        exitIndep = true;
                        connectors.pop();
                    }
                    else {
                        Semicolon single(arguments.front());
                        single.run(); // This doesn't report an exit status.
                        arguments.pop(); // Deallocating and getting rid of 
                        connectors.pop(); // A clean arg is a leak-free program!
                    }
                }
                else if (strcmp(connectors.front(), "||") == 0) { 
                    if (exitIndep) {
                        tmpArg = arguments.front();
                        arguments.pop();
                        DoubleBars doubl(tmpArg, arguments.front());
                        exitStatus = doubl.run();
                    }
                    else {
                        DoubleBars doubl(arguments.front());
                        exitStatus = doubl.run(exitStatus);
                    }
                    arguments.pop();
                    connectors.pop();
                    exitIndep = false;
                }
                else if (strcmp(connectors.front(), "&&") == 0) { 
                    if (exitIndep) {
                        tmpArg = arguments.front();
                        arguments.pop(); // This just gets rid of the pointer, not the actual value
                        Ampersand doubl(tmpArg, arguments.front());
                        exitStatus = doubl.run();
                    }
                    else {
                        Ampersand doubl(arguments.front()); // Everytime arguments are run from a class, the commands get destructed themselves
                        exitStatus = doubl.run(exitStatus);
                    }
                    arguments.pop();
                    connectors.pop();
                    exitIndep = false;
                }
                else if (strcmp(connectors.front(), "#") == 0) {
                    execute(arguments.front());
                    connectors.pop();
                    while (!arguments.empty()) arguments.pop(); // Holy moly kills the rest of the args
                    while (!connectors.empty()) connectors.pop(); // KILLS EVERYTHING
                }
                else {
                    arguments.pop();
                }
            }
            else {
                if (arguments.front() != '\0') execute(arguments.front());
                arguments.pop();
            }
        }

        while (!connectors.empty()) connectors.pop();
        while (!arguments.empty()) arguments.pop(); // If, for some reason, these two are not empty.
        while (!data.empty()) {
            delete[] data.front(); // This deallocates the entire command string. Connectors & arguments
            data.pop();
        }
        printInfo(login, host);
        cout << "$ ";
    }
    return 0;
}
