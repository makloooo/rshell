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

list<char*> parse(char cstrcmd[]) {
    char* token;
    list<char*> tokens;

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

void printQueue(list<char**> q) {
    for (int i = 0; !q.empty(); ++i) {
        cout << "[DEBUG] Argument #" << i << ": ";
        printArgs(q.front());
        q.pop_front();
        cout << endl;
    }
    return;
}

void printQueue(list<char*> q) {
    while (!q.empty()) {
        cout << q.front() << ' ';
        q.pop_front();
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

bool test(char* argv[]) {

    // Forks our process and stores its ID into a variable "child id"
    
    printLine(20);

    struct stat fileStat;

    cout << "[DEBUG] Statting "; printArgs(argv); cout << "!" << endl;
    if (argv[1][0] == '-') { // If this is a flag
        cout << "[DEBUG] Flag detected!" << endl;
        if (stat(argv[2], &fileStat) == 0) {
            if (argv[1][1] == 'e') return true;
            else if (argv[1][1] == 'f') return S_ISREG(fileStat.st_mode);
            else if (argv[1][1] == 'd') return S_ISDIR(fileStat.st_mode);
            else {
                cout << "test failed: invalid flag passed in\n";
                return false;
            }
        }
        else return false;
    }
    cout << "[DEBUG] No flag detected." << endl;

    printLine(20);

    return (stat(argv[1], &fileStat) == 0); 
}

Connector* buildTree(list<char**>& args, list<char*>& cons) {
    // Returns the head of the tree.
    // Allocated data here can be deleted during cleanup via preorder deletion.
    
    printLine(30);

    ConnectorFactory factory;
    // cout << "[DEBUG] Throwing "; printArgs(args.front()); cout << " into leftCmd." << endl;
    cout << "[DEBUG] Building left node branch." << endl;
    Connector* leftCmd = factory.createBranch(args, cons); 
    cout << "[DEBUG] Left node Subtree Built." << endl;
    // if (leftCmd != NULL) cout << "[DEBUG] leftCmd holds "; printArgs(leftCmd->getCmd()); cout << endl;
    if (args.empty()) {
        cout << "[DEBUG] args list held one element only, returning..." << endl;
        return leftCmd;
    }

    Connector* rightCmd = NULL; 
    Connector* currHead = NULL;
    
    while (!cons.empty()) {
        while (!args.empty()) {

            if (checkConnectors(cons.front()) == 1) { // If is hash, EVACUATE.
                while (!args.empty()) args.pop_front();
                while (!cons.empty()) cons.pop_front();
                break;
            }
            currHead = factory.createConnector(checkConnectors(cons.front()));
            cout << "[DEBUG] currHead speaking: "; currHead->identify();
            cons.pop_front();

            rightCmd = factory.createBranch(args, cons); // This is going to handle precedence.
            cout << "[DEBUG] Branch created." << endl;

            currHead->setLeft(leftCmd);
            currHead->setRight(rightCmd);
            cout << "[DEBUG] Statement: leftCmd = currHead" << endl;
            leftCmd = currHead; // Sets left leaf to be current head.
            cout << "[DEBUG] leftCmd: "; leftCmd->identify();
        }
        if (args.empty() && !cons.empty()) cons.pop_front();
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
    int flag = checkConnectors(argv);
    if (flag == 2 || flag == 3) return true;
    return false;
}

bool isAttached(char* argv) {
    if (strlen(argv) > 1) return false;
    else if (argv[strlen(argv) - 1] == ';') return true;
    return false;
}

bool isComment(char* argv) {
    return (argv[0] == '#') ? true : false;
}

bool isTestBegin(char* argv) {
    return (strcmp(argv, "[") == 0) ? true : false;
}

bool isTestEnd(char* argv) {
    if (strcmp(argv, "]") == 0) return true;
    else if (strcmp(argv, "])") == 0) return true;
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
