#include "helper.h"

void truncate(char*& argv) {
    argv[strlen(argv) - 1] = '\0';
    return;
}

void trim(char*& argv) {
    argv[0] = '\0';
    ++argv;
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

void printLine(int num) {
    cout << "[DEBUG] ";
    for (int i = 0; i < num; ++i) cout << '-';
    cout << endl;
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

/*
bool execute(char* args[]) {

    if (strcmp(args[0], "exit") == 0) exit(0);
    else if (strcmp(args[0], "test") == 0) test(args[]);

    int status;
    pid_t c_pid, pid; // child_processID, processID
    c_pid = fork(); 
    // Forks our process and stores its ID into a variable "child id"

    if (c_pid < 0) { // If the fork doesn't take place at all => It fails
        perror("Fork failed");
        exit(1);
    }
    else if (c_pid == 0) { 
        // If the fork succeeds, and this is the child process
        cout << "[DEBUG] --------------------" << endl;
        cout << "[DEBUG] I'm a child process." << endl;
        cout << "[DEBUG] Executing '"; printArgs(args); cout << "'!" << endl;
        execvp(args[0], args);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }
    else if (c_pid > 0) { 
        // If the fork succeeds, and this is the parent process
        cout << "[DEBUG] I'm the parent process." << endl;
        if ( (pid = wait(&status)) < 0) {
            perror("Something went wrong while waiting");
            exit(1);
        }
        cout << "[DEBUG] --------------------" << endl;
        cout << "[DEBUG] Child finished its job. Whew!" << endl;
    }
    // This should only be run if the command is valid.
    return (status == 0);
}
*/

bool test(char* argv[]) {

    int status;
    pid_t c_pid;
    pid_t pid; // child_processID, processID
    c_pid = fork(); 
    // Forks our process and stores its ID into a variable "child id"
    
    if (c_pid < 0) { // If the fork doesn't take place at all => It fails
        perror("Fork failed");
        exit(1);
    }
    else if (c_pid == 0) { 
        // If the fork succeeds, and this is the child process
        cout << "[DEBUG] --------------------" << endl;
        cout << "[DEBUG] I'm a child process." << endl;
        cout << "[DEBUG] Statting '"; printArgs(argv); cout << "'!" << endl;
        //stat(argv[0], argv);
        perror("stat failed");
        exit(EXIT_FAILURE);
    }
    else if (c_pid > 0) { 
        // If the fork succeeds, and this is the parent process
        cout << "[DEBUG] I'm the parent process." << endl;
        if ( (pid = wait(&status)) < 0) {
            perror("Something went wrong while waiting");
            exit(1);
        }
        cout << "[DEBUG] -------------------" << endl;
        cout << "[DEBUG] Child finished its job with exit status " 
             << status << '.' << endl;
    }
    cout << "[DEBUG] WIFEXITED yielded "; 
    (WIFEXITED(status)) ? cout << "true" : cout << "false"; 
    cout << endl;
    // This should only be run if the command is valid.
    return (status == 0);
}

/*
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
*/

Connector* buildTree(queue<char**>& args, queue<char*>& cons) {
    // Returns the head of the tree.
    // Allocated data here can be deleted during cleanup via preorder deletion.
    
    printLine(30);

    cout << "[DEBUG] Throwing "; printArgs(args.front()); cout << " into leftCmd." << endl;
    Connector* leftCmd = new Connector(args.front());
    args.pop(); // May lose address here, but is stored in new connector.

    if (leftCmd != NULL) cout << "[DEBUG] leftCmd holds "; printArgs(leftCmd->getCmd()); cout << endl;
    if (args.empty()) {
        cout << "[DEBUG] args queue held one element only, returning..." << endl;
        return leftCmd;
    }

    Connector* rightCmd = NULL; 
    Connector* currHead = NULL;
    
    ConnectorFactory factory;

    while (!cons.empty()) {
        while (!args.empty()) {

            if (checkConnectors(cons.front()) == 1) { // If is hash, EVACUATE.
                while (!args.empty()) args.pop();
                while (!cons.empty()) cons.pop();
                break;
            }

            rightCmd = factory.createBranch(args, cons); // This is going to handle precedence.
            cout << "[DEBUG] Branch created." << endl;
            currHead = factory.createConnector(checkConnectors(cons.front()));
            cout << "[DEBUG] currHead speaking: "; currHead->identify();
            cons.pop();
            currHead->setLeft(leftCmd);
            currHead->setRight(rightCmd);
            cout << "[DEBUG] Statement: leftCmd = currHead" << endl;
            leftCmd = currHead; // Sets left leaf to be current head.
            cout << "[DEBUG] leftCmd: "; leftCmd->identify();
        }
        if (args.empty() && !cons.empty()) cons.pop();
    }
   
    printLine(30);

    return currHead;
}

void printTree(Connector* node) {
    cout << "[DEBUG] I'm your current node. "; node->identify();
    if (node->getLeft() != NULL) {
        cout << "[DEBUG] I'm handing it over to my left branch!\n";
        printTree(node->getLeft());
    }
    if (node->getRight() != NULL) {
        cout << "[DEBUG] I'm now handing it over to my right branch!\n";
        printTree(node->getRight());
    }
}


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

bool hasStartParenthesis(char*& arg) {
    if (arg[0] == '(') {
        trim(arg);
        return true;
    }
    return false;
}

bool hasEndParenthesis(char**& args) {
    for (int i = 0; args[i] != NULL; ++i) {
        if (args[i][strlen(args[i]) - 1] == ')') {
            truncate(args[i]);
            return true;
        }
    }
    return false;
}
