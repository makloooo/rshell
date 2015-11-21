#include "connector.h"

/*Begin Definitions for 'connector'*/
Connector::Connector() {
    command[0] = '\0';
    left = NULL;
    right = NULL;
}

Connector::Connector(char* argv[]) {
    for (int i = 0; argv[i] != '\0'; ++i) {
        command[i] = argv[i];
        command[i+1] = '\0';
    }
    left = NULL;
    right = NULL;
}

Connector::~Connector() {}

bool Connector::execute(char* args[]) {

    if (strcmp(args[0], "exit") == 0) exit(0);
    else if (strcmp(args[0], "test") == 0) {
        // return test(args);
        bool success = test(args);
        cout << "[DEBUG] Test yielded "; (success) ? cout << "true" : cout << "false"; cout << endl;
        return success;
    }

    int status;
    pid_t c_pid, pid; // child_processID, processID
    c_pid = fork(); 
    // Forks our process and stores its ID into a variable "child id"

    // First fork a process.
    if (c_pid < 0) { 
        // If the fork doesn't take place at all => It fails
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    else if (c_pid == 0) { 
        // If the fork succeeds, and this is the child process
        cout << "[DEBUG] --------------------" << endl;
        cout << "[DEBUG] I'm a child process." << endl;
        cout << "[DEBUG] Executing "; printArgs(args); cout << endl;
        execvp(args[0], args);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }
    else if (c_pid > 0) { 
        // If the fork succeeds, and this is the parent process
        cout << "[DEBUG] I'm the parent process." << endl;
        if ( (pid = wait(&status)) < 0) {
            perror("wait failed");
            exit(EXIT_FAILURE);
        }
         cout << "[DEBUG] -------------------" << endl;
        cout << "[DEBUG] Child finished its job with exit status " 
             << status << '.' << endl;
    }
    cout << "[DEBUG] WIFEXITED yielded "; 
    (WIFEXITED(status)) ? cout << "true" : cout << "false"; 
    cout << endl;
    return (status == 0); // I'M CHEATING
}

bool Connector::run() {
    cout << "[DEBUG] Running my commands! By the way, "; this->identify();
    return execute(command);
}

void Connector::setLeft(Connector* l) {
    left = l;
}

void Connector::setRight(Connector* r) {
    right = r;
}

Connector* Connector::getLeft() {
    return left;
}

Connector* Connector::getRight() {
    return right;
}

bool Connector::hasLeft() {
    return (left != NULL);
}

bool Connector::hasRight() {
    return (right != NULL);
}

void Connector::destroyBranch(Connector* node) {
    // You should be passing in the right branch head
    // Preorder deletion
    if (node->left != NULL) destroyBranch(node->left);
    if (node->right != NULL) destroyBranch(node->right);
    delete node;
    return;
}

char** Connector::getCmd() {
    return command;
}

void Connector::identify() {
    cout << "I'm an argument!" << endl;
    cout << "[DEBUG] I have the following command: "; printArgs(command); cout << '!' << endl;
}
/*End definitions for 'Connector'*/

/* Begin definitions for 'Ampersand' */
Ampersand::Ampersand() {
    left = NULL;
    right = NULL;
}

Ampersand::Ampersand(Connector* l, Connector* r) {
    left = l;
    right = r;
}

Ampersand::~Ampersand() {
    left = NULL; // NOT VALUES, JUST POINTERS
    right = NULL;
}

bool Ampersand::run() {
    cout << "[DEBUG] Running my commands! By the way, "; this->identify();
    if (!left->run()) return false; // And don't run the next command
    cout << "[DEBUG] Running second command." << endl;
    return right->run();
}

void Ampersand::identify() {
    cout << "I'm an Ampersand connector!" << endl;
}
/* End definitions for 'Ampersand' */

/* Begin definitions for 'DoubleBars' */
DoubleBars::DoubleBars() {
    left = NULL;
    right = NULL;
}
DoubleBars::DoubleBars(Connector* l, Connector* r) {
    left = l;
    right = r;
}

DoubleBars::~DoubleBars() {
    left = NULL;
    right = NULL;
}

bool DoubleBars::run() {
    cout << "[DEBUG] Running my commands! By the way, "; this->identify();
    if (!left->run()) {
        return right->run();
    }
    return true; 
    // If cmd1 executed correctly:
    // The if statement should not occur and just return true.
}

void DoubleBars::identify() {
    cout << "I'm a DoubleBar connector!" << endl;
}
/* End definitions for 'Doublebars' */

/* Begin definitons for 'Semicolon' */
Semicolon::Semicolon() {
    left = NULL;
    right = NULL;
}

Semicolon::Semicolon(Connector* l, Connector* r) {
    left = l;
    right = r;
}

Semicolon::~Semicolon() {
    left = NULL;
    right = NULL;
}

bool Semicolon::run() {
    cout << "[DEBUG] Running my commands! By the way, "; this->identify();
    bool success = false;
    if (hasLeft()) success = left->run();
    if (hasRight()) return right->run(); // Should always run the next command.
    return success;
}

void Semicolon::identify() {
    cout << "I'm a Semicolon connector!" << endl;
}
/* End definitions for 'Semicolon' */

/* Begin definitions for 'Hash' */
Hash::Hash() {
    left = NULL;
    right = NULL;
}

Hash::Hash(Connector* l, Connector* r) {
    left = l;
    right = r;
}

Hash::~Hash() {
    left = NULL;
    right = NULL;
}

bool Hash::run() {
    left->run(); // Runs this...
    // FIXME: Then it should kill its parents and right children...
    // Sounds morbid.
    return false; // Placeholder
}

void Hash::identify() {
    cout << "I'm a Hash operator! ...:)" << endl;
}
/* End definitions for 'Hash' */

/* Begin definitions for 'Bracket'
Bracket::Bracket(char* argv[]) {
    for (int i = 0; argv[i] != '\0'; ++i) {
        command[i+1] = argv[i];
        command[i+2] = '\0';
    }
    left = NULL;
    right = NULL;
}

Bracket::~Bracket() {
    setNull(cmd1);
}

bool Bracket::run() {
    return test(cmd1);
}

bool Bracket::run(bool exitStatus) {
    return test(cmd1);
}
End definitions for 'Bracket' */

/* Begin definitions for 'ConnectorFactory' */
Connector* ConnectorFactory::createBranch(list<char**>& args, list<char*>& cons) {
    
    // This is almost the same algorithm as BuildTree()
    // If has more right branches, build via recursion.

    printLine(20);
    cout << "[DEBUG] Creating new Sub-Tree." << endl;

    Connector* subHead = NULL;
    Connector* left = NULL;
    Connector* right = NULL;
    
    ConnectorFactory factory;

    if (hasStartParenthesis(args.front()[0])) {
        hasEndParenthesis(args.front());
        left = new Connector(args.front());
        args.pop_front();
        cout << "[DEBUG] left: "; left->identify();
        for (int i = 0; !args.empty() && !hasEndParenthesis(args.front()); ++i) { // Input should not allow infinite loop here
            subHead = factory.createConnector(checkConnectors(cons.front()));
            cons.pop_front();
            right = factory.createBranch(args, cons); 
            subHead->setLeft(left);
            subHead->setRight(right);
            cout << "[DEBUG] Statement: left = subHead" << endl;
            left = subHead;
            cout << "[DEBUG] left: "; left->identify();
        }
        cout << "[DEBUG] args.front() is terminated by ')'" << endl;
        if (!args.empty()) {
            cout << "[DEBUG] Attaching argument to right leaf." << endl;
            right = factory.createBranch(args, cons);
            subHead = factory.createConnector(checkConnectors(cons.front()));
            cons.pop_front();
            subHead->setLeft(left);
            subHead->setRight(right);
            cout << "[DEBUG] Statement: left = subHead" << endl;
            left = subHead;
            cout << "[DEBUG] left: "; left->identify();
        }
        else {
            cout << "[DEBUG] No argument to attach to right node - remaining NULL." << endl;
            subHead = left;
        }
    }
    else {
        cout << "[DEBUG] Doesn't have precedence.\n[DEBUG] Creating right leaf." << endl;
        subHead = new Connector(args.front());
        cout << "[DEBUG] Right leaf holds "; printArgs(subHead->getCmd()); cout << endl;
        args.pop_front();
    }

    printLine(20);

    return subHead; 
}

Connector* ConnectorFactory::createConnector(int flag) {
    if (flag == 0) return new Semicolon;
    else if (flag == 1) return new Hash;
    else if (flag == 2) return new DoubleBars;
    else if (flag == 3) return new Ampersand;
    else return NULL;
}
/* End definitions for 'ConnectorFactory' */
