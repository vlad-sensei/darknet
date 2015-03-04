#include "ui.h"

#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>

#include "boost/mpl/list.hpp"
#include "boost/mpl/for_each.hpp"

//GNU readline library
#include <readline/readline.h>
#include <readline/history.h>

#include "command.h"


static char** my_completion(const char*, int ,int);
char* my_generator(const char*,int);
char* dupstr (char*);
void* xmalloc (int);

char** cmd_list;

unordered_map<string,shared_ptr<Command>> command_map;

template<class U>
void init_cmd_map(){
  shared_ptr<U> connCmd(new U(move(vector<string>(0))));
  for(auto &cmd_word : connCmd->command_word()){
    command_map.emplace(cmd_word,connCmd);
  }
}

struct init_cmd_map_wrapper {
    template<typename U> void operator()(U) {
        init_cmd_map<U>();
    }
};

UI::UI() {

  boost::mpl::for_each<command_list>(init_cmd_map_wrapper());


  for(auto key : command_map) {
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

  printf(" ~~~ welcome to darknet ~~~\n");
  thread([this](){get_raw_input();}).detach();
}

void UI::get_raw_input(){

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

    process_raw_input(input);

    free(line);
  }

  //Old code
//  string raw_input;
//  cout << "> ";
//  while(getline(cin,raw_input)){
//    thread([this,raw_input](){
//      try{
//        process_raw_input(move(raw_input));
//      } catch(std::exception&e){
//        debug("*** troll user put? : %s",e.what());
//      }
//      cout << "\n> ";
//    }).detach();
//  }
}

void UI::process_raw_input(string raw_input){

  //Split string and put into array
  istringstream ss(raw_input);
  istream_iterator<string> begin(ss), end;
  vector<string> cmd_args(begin, end);

  if(cmd_args.size()==0){
    return;
  }

  string cmd_str = (string)cmd_args[0];

    if(find(command_keys.begin(), command_keys.end(), cmd_str) == command_keys.end()) {
      // Typed command not found
      cout << "Invalid command: Not found" << endl;
      return;
    }

  shared_ptr<Command> cmd = command_map[cmd_str];

  cmd->set_args(move(cmd_args));
  cmd->execute();
}

static char** my_completion( const char * text , int start,  int end)
{
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
