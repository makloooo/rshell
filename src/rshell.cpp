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
        char* token = '\0';
        deque<char*> tokens;

        cstrcmd = new char[command.size() + 1]; // This data is universal. It will be used until deallocated.
        strcpy(cstrcmd, command.c_str());

        token = strtok(cstrcmd, " ");
        while (token != '\0') {
            // cout << token << " Tokenized!" << endl;
            tokens.push_back(token);
            token = strtok('\0', " ");
        }

        // cout << "[DEBUG] Queue contents: "; printQueue(tokens);

        if (isDblConnector(tokens.back())) {
            tokens.pop_back();
            cout << "Terminated without command -- ignoring leading connector." << endl;
        }
        /*
        while (isDblConnector(tokens.back())) { // If the stupid user ended with a connector
            queue<char*> tmpCmds;

            cout << "> ";
            getline(cin, command);
            copyQueue(tmpCmds, parse(command));
            while (!tmpCmds.empty()) {
                tokens.push(tmpCmds.front());
                tmpCmds.pop();
            }

            cout << "[DEBUG] Queue contents: "; printQueue(tokens);
        }
        */

        // cout << "[DEBUG] Creating both queues..." << endl;
        int last; // There definitely has to be a better way to do this...
        char* cmd[128][256]; // cmd is a list of pointers to the beginning of each token of the universal data
        int n = 0;
        for (int i = 0; !tokens.empty(); ++i) {
            cutEndSpaces(tokens.front());
            if (isConnector(tokens.front())) {
                if (!isDblConnector(tokens.front())) { // Pesky semicolons being attached...
                    truncate(tokens.front()); // e.g. -a; >> -a
                    // cout << "[DEBUG] Assigning cmd[" << i << "] value of '"; printArg(tokens.front()); cout << '\'' << endl;
                    cmd[n][i] = tokens.front(); // cmd[i] gets -a, contains pointer to the first character of that value, '-'
                    // cout << "[DEBUG] Assigned cmd[" << i << "] value of '"; printArg(cmd[n][i]); cout << '\'' << endl;
                    tokens.front() = new char; // tokens.front() gets a pointer to new cString
                    // FIXME: Find out where to correctly deallocate this
                    strcpy(tokens.front(), ";\0"); // strcpy sets its values to be ";\0"
                    ++i; // Progressing to the next value of cmd, since it was assigned in this instance
                }
                cmd[n][i] = '\0';
                // cout << "[DEBUG] Pushing in '"; printArgs(cmd[n]); cout << '\'' << endl;
                
                arguments.push(cmd[n]); // If we ran into a connector, that means we have a full argument already
                // Arguments gets a pointer to a block of cstrings
                // cout << "[DEBUG] Pushed in '"; printArgs(arguments.back()); cout << '\'' << endl;
                connectors.push(tokens.front());
              
                ++n;
                i = -1; // Reset argc
            }
            else {
                // cout << "[DEBUG] Assigning cmd[" << i << "] value of '"; printArg(tokens.front()); cout << '\'' << endl;
                cmd[n][i] = tokens.front(); // Assigns cmd[i] to pointer at tokens.front(). Tokens contains cstrings
                // cout << "[DEBUG] Assigned cmd[" << i << "] value of '"; printArg(cmd[n][i]); cout << '\'' << endl;
            }
            // cout << "[DEBUG] Popping '" << tokens.front() << '\'' << endl;
            tokens.pop_front();
            last = i;
        }
        if (cmd[n][0] != '\0') { // Push in the last argument
            cmd[n][last+1] = '\0';
            // cout << "[DEBUG] Pushing in tail argument '"; printArgs(cmd[n]); cout << '\'' << endl;
            arguments.push(cmd[n]);
            // cout << "[DEBUG] Pushed in tail argument '"; printArgs(arguments.back()); cout << '\'' << endl;
        }
        
        

        // Debug printing
        /*
        cout << "[DEBUG] Size of 'arguments': " << arguments.size() << endl;
        cout << "[DEBUG] Size of 'connectors': " << connectors.size() << endl;
        cout << "[DEBUG] Contents of 'arguments': " << endl; printQueue(arguments);
        cout << "[DEBUG] Contents of 'connectors': "; printQueue(connectors);
        */

        // Runs through both queues simutaneously
        // After the first run, logic is checked parallel to arguments run
        bool exitStatus;
        bool exitIndep = true; // There's probably a way better way to do this...
        char** tmpArg;
        while (!arguments.empty()) {
            if (!connectors.empty()) {
                if (strcmp(connectors.front(), ";") == 0) {
                    Semicolon single(arguments.front());
                    single.run(); // This doesn't report an exit status.
                    arguments.pop(); // Deallocating and getting rid of 
                    connectors.pop(); // A clean arg is a leak-free program!
                    exitIndep = true;
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
                    arguments.pop(); arguments.pop(); // Basically ignore the next arg no matter what
                }
                else {
                    // cout << "[DEBUG] Connector '" << connectors.front() << "' unrecognized." << endl;
                    arguments.pop();
                }
            }
            else {
                // cout << "[DEBUG] Final execution." << endl;
                execute(arguments.front());
                arguments.pop();
            }
        }

        delete[] cstrcmd; // This deallocates the entire command string. Connectors & arguments
        printInfo(login, host);
        cout << "$ ";
    }

    // Parse it, then throw it into a container.
    // You need an abstract base class, which pure virtual is the run function
    // Each connector is its own derived class of the connector abstract class

    return 0;
}
