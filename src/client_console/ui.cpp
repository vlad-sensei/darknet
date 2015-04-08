#include "ui.h"

#include <thread>
#include <mutex>
#include <readline/readline.h>
#include <readline/history.h>

UI_ptr ui;


void UI::run(uint16_t ui_port){
  Connection_initiator_base::connect("localhost",ui_port);

  thread input_thread([this](){
    get_text_input();
  });

  thread network_thread([this](){
    Connection_initiator_base::run();
  });
  network_thread.join();
  input_thread.join();
}

void UI::handle_new_connection(tcp::socket socket){
  connection = Connection_ptr(new Connection(socket));
  connection->init();
}

void UI::echo(const string &msg){
  safe_printf("%s\n", msg);
}

// ~~~~~~~~~~~~ text input ~~~~~~~~~~~~

void UI::get_text_input(){

  init_readline();

  safe_printf(" ~~~ welcome to darknet ~~~\n");

  while(1){
    /*
     * Uncomment the line below and 'rl_bind_key' line in
     * 'my_completion' to disable 'ls' behavior of auto-complete.
     * 'ls'-behavior is shown when there is a space to the left in cmd-line.
     */
    //    rl_bind_key('\t',rl_complete);

    char* line = readline("> ");

    if(!line) break;
    if(*line){
      add_history(line);

      string input(line);

      connection->text_command(input);

      free(line);

/*
      //wait for return message of sent command
      unique_lock<mutex> lk(connection->m);
      connection->cv.wait(lk);
*/

      //handle properly with ncurses
    }else{
      free(line);
    }
  }
}


static char** my_completion(const char*, int ,int);
char* my_generator(const char*,int);
char* dupstr (char*);
void* xmalloc (int);

char** cmd_list;
vector<char*> command_keys;

void UI::init_readline(){


  for(const auto& key : command_list) {
    char* a = new char[key.size()+1];
    a[key.size()] = '\0';
    memcpy(a,key.c_str(),key.size());
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
