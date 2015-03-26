//TODO: !!!!!! deprecate readline with something more c++:ish

#include "ui.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <iterator>
#include <readline/readline.h>
#include <readline/history.h>

#include "core.h"



UI::UI() {
  init_commands();
  init_readline();
  safe_printf(" ~~~ welcome to darknet ~~~\n");
  thread([this](){get_text_input();}).detach();
}


void UI::run(){
  Connection_initiator_base::set_port(DEFAULT_UI_LISTEN_PORT);
  Connection_initiator_base::start_listen();
  Connection_initiator_base::run();
}


void UI::handle_new_connection(tcp::socket socket){
  // do some verification if needed
  spawn_client(socket);
}

void UI::spawn_client(tcp::socket &socket){
  w_lock l(clients_mtx);
  const peer_id_t& uid = ++data.current_ui_peer;
  data.clients[uid] = UI_client_ptr(new UI_client(socket));
  data.clients[uid]->init();
}


static char** my_completion(const char*, int ,int);
char* my_generator(const char*,int);
char* dupstr (char*);
void* xmalloc (int);

char** cmd_list;

void UI::get_text_input(){

  while(1){
    /*
     * Uncomment the line below and 'rl_bind_key' line in
     * 'my_completion' to disable 'ls' behavior of auto-complete.
     * 'ls'-behavior is shown when there is a space to the left in cmd-line.
     */
    //    rl_bind_key('\t',rl_complete);

    char* line = readline("> ");

    if(!line) break;
    if(*line) add_history(line);

    string input(line);

    process_text_input(input);

    free(line);
  }
  /*
  string text_input;
  while(getline(cin,text_input)){
    thread([this,text_input](){
      process_text_input(move(text_input));
    }).detach();
    safe_printf(">");
    cout.flush();
  }
*/
}

void UI::process_text_input(string text_input){
  istringstream ss(text_input);
  //istream_iterator<string> begin(ss), end;
  //vector<string> args(begin, end;
  istream_iterator<string> begin(ss), end;
  vector<string> cmd_args(begin, end);
  if(cmd_args.empty()){
    return;
  }
  r_lock(commands_mtx);
  if(!data.command_exists(cmd_args[0])){
    safe_printf("No such command: %s\n", cmd_args[0]);
    return;
  }
  data.commands[cmd_args[0]]->exec(cmd_args);
}

void UI::Command::exec(const vector<string> &args){
  if(args.size()<minargc_||args.size()>maxargc_){
    printf("Wrong argument count\n");
    help();
    return;
  }
  try{
    execute_(args);
  } catch(exception&e){
    printf(" *** error processing command : %s\n",e.what());
  }
}

//TODO: make sure there are no copies
void UI::init_command(const vector<string> &key_words, const cmd_lambda_t &execute, const string &help_text, const unsigned &minargc, const unsigned &maxargc){
  Command_ptr cmd = Command_ptr(new Command(execute, help_text, minargc, maxargc));
  w_lock w(commands_mtx);
  for(const string& key_word : key_words) data.commands[key_word]=cmd;
}

//TODO: check if that this does not lead to any race condition when "this" (UI/Core) is destroyed
void UI::init_commands(){
  init_command({"c", "connect"},
               [this](const vector<string>& args){
    //TODO: find a better way to parse args, perhaps a template of some kind
    string peer;
    uint16_t port = DEFAULT_LISTEN_PORT;
    try{
      peer = args[1];
      if(args.size()>2) port = stoul(args[2]);
    } catch(exception& e){
      handle_invalid_args(e);
      return;
    }
    core->connect(peer,port);
  },
  "connect PEER_IP [PORT]",
  2,3);

  init_command({"br", "broadcast"},
               [this](const vector<string>& args){
    string msg;
    try{
      msg = args[1];
    } catch(exception& e){
      handle_invalid_args(e);
      return;
    }
    core->broadcast_echo(msg);
  },
  "broadcast MESSAGE",
  2,2);

  init_command({"exit", "quit"},
               [this](const vector<string>& args){
    (void)args;
    safe_printf("Exiting darknet...");
    exit(0);
  },
  "'exit' or 'quit'",
  1,1);

}

vector<char*> command_keys;

void UI::init_readline(){
  r_lock(commands_mtx);
  for(auto key : data.commands) {
    string str = key.first;
    char* a = new char[str.size()+1];
    a[str.size()] = '\0';
    memcpy(a,str.c_str(),str.size());
    command_keys.push_back(a);
  }
  //Might be better way to build 'cmd_list'
  cmd_list = &command_keys[0];
  // Used for custom auto-complete with 'readline'
  rl_attempted_completion_function = my_completion;
  rl_bind_key('\t',rl_complete);
}

static char** my_completion( const char * text , int start,  int end)
{
  (void)end;
  char** matches;

  matches = (char**)NULL;

  if (start == 0){
    matches = rl_completion_matches ((char*)text, &my_generator);
  }else{
    //        rl_bind_key('\t',rl_insert); //Disables 'ls'-behavior
  }

  return (matches);

}

char* my_generator(const char* text, int state)
{
  static int list_index, len;
  char *name;

  if (!state) {
    list_index = 0;
    len = strlen (text);
  }

  while ((name = cmd_list[list_index])) {
    list_index++;

    if (strncmp (name, text, len) == 0)
      return (dupstr(name));
  }

  /* If no names matched, then return NULL. */
  return ((char *)NULL);

}

char* dupstr (char* s) {
  char* r;

  r = (char*) xmalloc((strlen (s) + 1));
  strcpy (r, s);
  return (r);
}

void* xmalloc (int size)
{
  void* buf;

  buf = malloc (size);
  if (!buf) {
    fprintf (stderr, "Error: Out of memory. Exiting.\n");
    exit (1);
  }

  return buf;
}
