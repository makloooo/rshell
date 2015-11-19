#include "include.h"

#include "helper.h"
#include "check.h"
#include "connector.h"

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

        cstrcmd = new char[command.size() + 1]; 
        // This data is universal. It will be used until deallocated.

        strcpy(cstrcmd, command.c_str());
        data.push(cstrcmd); 
        // This is to hold locations of the data to deallocate later
        
        tokens = parse(cstrcmd);

        /*if (isDblConnector(tokens.back())) {
            tokens.pop_back();
            cout << "Terminated without command -- ignoring leading connector."
                 << endl;
        }*/

        while (isDblConnector(tokens.back())) { 
            // If the stupid user ended with a connector
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

        printLine(40);

        // Creating the two argument and connector queues
        int last; // There definitely has to be a better way to do this.
        char* cmd[128][256]; 
        // cmd is a list of pointers to the beginning of each token of the 
        // universal data
        int n = 0;
        for (int i = 0; !tokens.empty(); ++i) {
            cutEndSpaces(tokens.front());
            if (isConnector(tokens.front())) {
                if (isAttached(tokens.front())) { 
                    // Pesky semicolons being attached...
                    truncate(tokens.front()); 
                    cmd[n][i] = tokens.front(); 
                    tokens.front() = new char[1]; 
                    data.push(tokens.front());
                    strcpy(tokens.front(), ";\0"); 
                    ++i; // Progressing to the next value of cmd
                }
                cmd[n][i] = '\0'; // Null terminating cmd
                arguments.push(cmd[n]); 
                // If we ran into a connector, means we have a full argument.
                connectors.push(tokens.front());
              
                ++n;
                i = -1; // Reset argc
            }
            else {
                cmd[n][i] = tokens.front(); 
                // Assigns cmd[i] to pointer at tokens.front(). 
                // Tokens contains cstrings
            }
            tokens.pop_front();
            last = i;
        }
        if (cmd[n][0] != '\0') { // Push in the last argument
            cmd[n][last + 1] = '\0';
            arguments.push(cmd[n]);
        }

        // Debug printing:
        cout << "[DEBUG] Contents of 'arguments': " << endl; printQueue(arguments);
        cout << "[DEBUG] Contents of 'connectors': "; printQueue(connectors);

        // How to build a tree out of two queues?
        Connector* head = NULL;
        head = buildTree(arguments, connectors);
        cout << "[DEBUG] Tree built successfully... possibly!" << endl;
        cout << "[DEBUG] head speaking: "; head->identify();
        printLine(20); printTree(head); printLine(20);
        head->run();

        while (!connectors.empty()) connectors.pop();
        while (!arguments.empty()) arguments.pop(); 
        // If, for some reason, these two are not empty.
        while (!data.empty()) {
            delete[] data.front(); 
            // This deallocates the entire command string.
            data.pop();
        }
        head->destroyBranch(head);
        printLine(40);
        printInfo(login, host);
        cout << "$ ";
    }
    return 0;
}
