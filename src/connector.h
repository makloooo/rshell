#ifndef CONNECTOR_H
#define CONNECTOR_H

#include "include.h"
#include "helper.h"

class Connector {
    protected:
        char* cmd1[256]; // Holds commands
        char* cmd2[256];
        
        bool execute(char* args[]);
    public:
        virtual bool run() = 0;
        virtual bool run(bool exitStatus) = 0;
};

class Ampersand : public Connector {
    public:
        Ampersand(char* first[]);
        Ampersand(char* first[], char* second[]);
        ~Ampersand();
        bool run();        
        bool run(bool exitStatus);
};

class DoubleBars : public Connector {
    public:
        DoubleBars(char* first[]);
        DoubleBars(char* first[], char* second[]);
        ~DoubleBars();        
        bool run();
        bool run(bool exitStatus);
};

class Semicolon : public Connector {
    public:
        Semicolon(char* first[]);
        ~Semicolon();
        bool run();
        bool run(bool exitStatus);
};

#endif
