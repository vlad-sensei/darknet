#ifndef COMMAND_H
#define COMMAND_H

#include "glob.h"
#include "core.h"

extern vector<char*> command_keys;

class Command;

extern unordered_map<string,shared_ptr<Command>> command_map;


class Command {
public:
  //    virtual ~Command(); //Needed?
  virtual bool verify_args()=0;
  virtual bool execute()=0;
  virtual string help_text()=0;
  virtual vector<string> command_word()=0;
  void set_args(vector<string>&& cmd_args){args=cmd_args;}
protected:
  vector<string> args;
};


class Connect: public Command{
public:                     //don't have super constructor to call
  Connect(){}
  Connect(vector<string>&& cmd_args){args=cmd_args;}
  bool verify_args();
  bool execute();
  string help_text();
  vector<string> command_word(){vector<string> vec {"connect"};return vec;}
};

class Disconnect: public Command{
public:
  Disconnect(){}
  Disconnect(vector<string>&& cmd_args){args=cmd_args;}
  bool verify_args();
  bool execute();
  string help_text();
  vector<string> command_word(){vector<string> vec {"disconnect"};return vec;}
};

class Broadcast: public Command{
public:
  Broadcast(){}
  Broadcast(vector<string>&& cmd_args){args=cmd_args;}
  bool verify_args();
  bool execute();
  string help_text();
  vector<string> command_word(){vector<string> vec {"broadcast","bc"};return vec;}
};

class Fetch: public Command{
public:
  Fetch(){}
  Fetch(vector<string>&& cmd_args){args=cmd_args;}
  bool verify_args();
  bool execute();
  string help_text();
  vector<string> command_word(){vector<string> vec {"fetch"};return vec;}
};

class Metalist: public Command{
public:
  Metalist(){}
  Metalist(vector<string>&& cmd_args){args=cmd_args;}
  bool verify_args();
  bool execute();
  string help_text();
  vector<string> command_word(){return cmd_word;}
private:
  vector<string> cmd_word {"metalist"};
};

class Encrypt: public Command{
public:
  Encrypt(){}
  Encrypt(vector<string>&& cmd_args){args=cmd_args;}
  bool verify_args();
  bool execute();
  string help_text();
  vector<string> command_word(){vector<string> vec {"encrypt"};return vec;}
};

class Decrypt: public Command{
public:
  Decrypt(){}
  Decrypt(vector<string>&& cmd_args){args=cmd_args;}
  bool verify_args();
  bool execute();
  string help_text();
  vector<string> command_word(){vector<string> vec {"decrypt"};return vec;}
};

class Help: public Command{
public:
  Help(){}
  Help(vector<string>&& cmd_args){args=cmd_args;}
  bool verify_args();
  bool execute();
  string help_text();
  vector<string> command_word(){vector<string> vec {"help","?"};return vec;}
};

class Quit: public Command{
public:
  Quit(){}
  Quit(vector<string>&& cmd_args){args=cmd_args;}
  bool verify_args(){return true;}
  bool execute();
  string help_text();
  vector<string> command_word(){vector<string> vec {"quit","exit"};return vec;}
};

typedef boost::mpl::list<Connect,Disconnect,Broadcast,Fetch,Metalist,Encrypt,Decrypt,Help,Quit> command_list;

#endif // COMMAND_H

