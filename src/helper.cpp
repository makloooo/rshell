#include "helper.h"

void truncate(char* argv) {
    argv[strlen(argv) - 1] = '\0';
    return;
}

deque<char*> parse(char cstrcmd[]) {
    char* token;
    deque<char*> tokens;

    token = strtok(cstrcmd, " ");
    while (token != '\0') {
        // cout << token << " Tokenized!" << endl;
        tokens.push_back(token);
        token = strtok('\0', " ");
    }
    return tokens;
}

void printArg(char* argv) {
    for (int i = 0; argv[i] != '\0'; ++i) {
        cout << argv[i];
    }
    return;
}

void printArgs(char* argv[]) {
   for (int i = 0; argv[i] != '\0'; ++i) {
        
        cout << '\''; printArg(argv[i]); cout << '\'';
        if (argv[i+1] != '\0') cout << ' ';
    }
}

void printInfo(char* login, char host[]) {
    printArg(login); 
    cout << '@';
    printArg(host);
    return;
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
    c_pid = fork(); 
    // Forks our process and stores its ID into a variable "child id"

    if (strcmp(args[0], "exit") == 0) exit(0);

    if (c_pid < 0) { // If the fork doesn't take place at all => It fails
        perror("Fork failed");
        exit(1);
    }
    else if (c_pid == 0) { 
        // If the fork succeeds, and this is the child process
        // cout << "[DEBUG] --------------------" << endl;
        // cout << "[DEBUG] I'm a child process." << endl;
        // cout << "[DEBUG] Executing '"; printArgs(args); cout << "'!" << endl;
        execvp(args[0], args);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }
    else if (c_pid > 0) { 
        // If the fork succeeds, and this is the parent process
        // cout << "[DEBUG] I'm the parent process." << endl;
        if ( (pid = wait(&status)) < 0) {
            perror("Something went wrong while waiting");
            exit(1);
        }
        // cout << "[DEBUG] --------------------" << endl;
        // cout << "[DEBUG] Child finished its job. Whew!" << endl;
    }
    // This should only be run if the command is valid.
    return (status == 0);
}

void runCommands(queue<char**> arguments, queue<char*> connectors) {
    // Runs through both queues simutaneously
    // After the first run, logic is checked parallel to arguments run
    bool exitStatus;
    bool exitIndep = true; // There's probably a way better way to do this.
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
                    arguments.pop(); 
                    // This just gets rid of the pointer
                    // not the actual value
                    Ampersand doubl(tmpArg, arguments.front());
                    exitStatus = doubl.run();
                }
                else {
                    Ampersand doubl(arguments.front()); 
                    // Everytime arguments are run from a class
                    // the commands get destructed themselves
                    exitStatus = doubl.run(exitStatus);
                }
                arguments.pop();
                connectors.pop();
                exitIndep = false;
            }
            else if (strcmp(connectors.front(), "#") == 0) {
                execute(arguments.front());
                connectors.pop();
                while (!arguments.empty()) arguments.pop(); 
                while (!connectors.empty()) connectors.pop();
                // This clause completely kills the rest of everything.
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
    return;
}
