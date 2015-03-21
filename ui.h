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
typedef function<void (const vector<string>&args)> cmd_lambda_t;

class UI{
public:
  UI();
private:
  void init_commands();
  void get_text_input();
  void process_text_input(string text_input);

  class Command{
  public:
    Command(const cmd_lambda_t & execute,const string& help_text = DEFAULT_COMMAND_HELP_TEXT, const unsigned& minargc=1, const unsigned& maxargc=-1):
      execute_(execute), help_text_(help_text), minargc_(minargc), maxargc_(maxargc){}
    void exec(const vector<string>& args);
    inline void help() {printf("%s\n",help_text_.c_str());}
  private:
    Command();
    const cmd_lambda_t execute_;
    const string help_text_;
    const unsigned minargc_, maxargc_;
  };
  //in case multiple keys want to point to same command
  typedef shared_ptr<Command> Command_ptr;

  unordered_map<string, Command_ptr> cmd;


  //virtual voids that must be in core
private:
  inline void handle_invalid_args(const exception& e) {safe_printf("invalid arguments : %s", e.what());}
  void init_command(const vector<string>& key_words,const cmd_lambda_t& execute,const string& help_text = DEFAULT_COMMAND_HELP_TEXT, const unsigned& minargc=1, const unsigned& maxargc=-1);
  virtual void connect(const string& addr, const uint16_t& port) =0;
  virtual void broadcast_echo(const string& msg)=0;
  struct {
    unordered_map<string, Command_ptr> commands;
    inline bool command_exists(const string& cmd) {return commands.find(cmd)!=commands.end();}
  } data;
  rw_mutex commands_mtx;
};

#endif // UI_H
