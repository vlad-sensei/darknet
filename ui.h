#ifndef UI_H
#define UI_H

/*
 * UI
 *
 * UI provides user input API which
 * communicate with Core.
 */

#include <memory>
#include <thread>
#include <vector>
#include <sstream>
#include <unordered_map>

#include "glob.h"

#define DEFAULT_COMMAND_HELP_TEXT "No text avaible for this command"

class UI{
public:
  UI();
private:
  void init_commands();
  void get_text_input();
  void process_text_input(string text_input);

  class Command{
  public:
    Command(function<void (const vector<string>&args)>&& execute,string&& help_text = DEFAULT_COMMAND_HELP_TEXT, const unsigned& minargc=1, const unsigned& maxargc=-1):
      execute_(execute), help_text_(help_text), minargc_(minargc), maxargc_(maxargc){}
    void exec(const vector<string>& args);
    inline void help() {printf("%s\n",help_text_.c_str());}
  private:
    Command();
    function <void (const vector<string>& args)> execute_;
    const string help_text_;
    const unsigned minargc_, maxargc_;
  };
  //in case multiple keys want to point on same command
  typedef shared_ptr<Command> Command_ptr;

  unordered_map<string, Command_ptr> cmd;


  //virtual voids that must be in core
private:
  virtual void connect(const string& addr, const uint16_t& port) =0;
  virtual void broadcast_echo(const string& msg)=0;
};

#endif // UI_H
