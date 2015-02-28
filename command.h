#ifndef COMMAND_H
#define COMMAND_H

#include <stdbool.h>
#include <iostream>
//#include <type_traits>

#include "glob.h"

#include "core.h"

class Command {
public:
//    virtual ~Command(); //Needed?
    virtual bool verify_args()=0;
    virtual bool execute()=0;
    virtual string help_text()=0;
protected:
    vector<string>* args;
};


class Connect: public Command{
public:
    Connect(vector<string>* cmd_args){args=cmd_args;}
//    Connect(vector<string>* cmd_args): Command(cmd_args){}
    bool verify_args();
    bool execute();
    string help_text();
};

class Disconnect: public Command{
public:
    Disconnect(vector<string>* cmd_args){args=cmd_args;}
    bool verify_args();
    bool execute();
    string help_text();
};

class Broadcast: public Command{
public:
    Broadcast(vector<string>* cmd_args){args=cmd_args;}
    bool verify_args();
    bool execute();
    string help_text();
};

class Fetch: public Command{
public:
    Fetch(vector<string>* cmd_args){args=cmd_args;}
    bool verify_args();
    bool execute();
    string help_text();
};


class Encrypt: public Command{
public:
    Encrypt(vector<string>* cmd_args){args=cmd_args;}
    bool verify_args();
    bool execute();
    string help_text();
};

class Decrypt: public Command{
public:
    Decrypt(vector<string>* cmd_args){args=cmd_args;}
    bool verify_args();
    bool execute();
    string help_text();
};

class Help: public Command{
public:
    Help(vector<string>* cmd_args){args=cmd_args;}
    bool verify_args(){return true;}
    bool execute();
    string help_text();
};

class Quit: public Command{
public:
    Quit(vector<string>* cmd_args){args=cmd_args;}
    bool verify_args(){return true;}
    bool execute();
    string help_text();
};


//template<class T>
//typename enable_if<is_base_of<Command, T>::value, T>::type
//create_command(vector<string>* cmd_args){
//    return T(cmd_args);
//}


#endif // COMMAND_H

