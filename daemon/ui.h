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
#include "connection_initiator_base.h"
#include "ui_client.h"

#define DEFAULT_COMMAND_HELP_TEXT "No text available for this command"
typedef function<string (const vector<string>&args)> cmd_lambda_t;

class UI;
typedef unique_ptr<UI> UI_ptr;

class UI : Connection_initiator_base {
public:
  UI();
  void run();
  string process_text_input(string& text_input);
private:

  void handle_new_connection(tcp::socket socket);
  void spawn_client(tcp::socket& socket);

  void init_commands();

  class Command{
  public:
    Command(const cmd_lambda_t & execute,const string& help_text = DEFAULT_COMMAND_HELP_TEXT, const unsigned& minargc=1, const unsigned& maxargc=-1):
      execute_(execute), help_text_(help_text), minargc_(minargc), maxargc_(maxargc){}
    string exec(const vector<string>& args);
    inline string help() {return help_text_;}
  private:
    Command();
    const cmd_lambda_t execute_;
    const string help_text_;
    const unsigned minargc_, maxargc_;
  };
  //in case multiple keys want to point to same command
  typedef shared_ptr<Command> Command_ptr;

  unordered_map<string, Command_ptr> cmd;

  inline void handle_invalid_args(const exception& e) {safe_printf("invalid arguments : %s", e.what());}
  void init_command(const vector<string>& key_words,const cmd_lambda_t& execute,const string& help_text = DEFAULT_COMMAND_HELP_TEXT, const unsigned& minargc=1, const unsigned& maxargc=-1);


  struct {
    peer_id_t current_ui_peer=0;
    unordered_map<peer_id_t,UI_client_ptr> clients;

    unordered_map<string, Command_ptr> commands;
    inline bool command_exists(const string& cmd) {return commands.find(cmd)!=commands.end();}
  } data;
  rw_mutex commands_mtx, clients_mtx;
};

#endif // UI_H
