#include "ui.h"

#include <sstream>
#include <iostream>
#include <fstream>
#include <iterator>

#include "core.h"

Ui::Ui() {
  init_commands();
}


void Ui::run(){
  Connection_initiator_base::start_listen();
  Connection_initiator_base::run();
}

void Ui::handle_new_connection(socket_ptr socket){
  // do some verification if needed
  spawn_client(socket);
}

void Ui::spawn_client(socket_ptr &socket){
  w_lock l(clients_mtx);
  const peer_id_t& uid = ++data.current_ui_peer;
  data.clients[uid] = Ui_client_ptr(new Ui_client(socket));

  data.clients[uid]->init();
}

Msg_ptr Ui::process_text_input(const string& text_input){
  istringstream ss(text_input);
  istream_iterator<string> begin(ss), end;
  vector<string> cmd_args(begin, end);
  if(cmd_args.empty()){
    return Message::echo("No arguments");
  }
  Commands cmd_enum = command_map[cmd_args[0]];

  r_lock(commands_mtx);
  if(!data.command_exists(cmd_enum)){
    return Message::echo("No such command: " + cmd_args[0]);
  }
  return data.commands[cmd_enum]->exec(cmd_args);
}

Msg_ptr Ui::Command::exec(const vector<string> &args){
  if(args.size()<minargc_||args.size()>maxargc_){
    return Message::echo("Wrong argument count\n"+help());
  }
  try{
    return execute_(args);
  } catch(exception&e){
    safe_printf(" *** error processing command : %s\n",e.what());
  }
  return Message::echo("Error processing command");
}

//TODO: make sure there are no copies
void Ui::init_command(const Commands cmd_enum, const cmd_lambda_t &execute, const string &help_text, const unsigned &minargc, const unsigned &maxargc){
  Command_ptr cmd = Command_ptr(new Command(execute, help_text, minargc, maxargc));
  w_lock w(commands_mtx);
  data.commands[cmd_enum] = cmd;
  // for(const string& key_word : key_words) data.commands[key_word]=cmd;
}

//TODO: check if that this does not lead to any race condition when "this" (Ui/Core) is destroyed
void Ui::init_commands(){
  init_command(Commands::CMD_CONNECT,
               [this](const vector<string>& args){
    //TODO: find a better way to parse args, perhaps a template of some kind
    string peer;
    uint16_t port = DEFAULT_LISTEN_PORT;
    try{
      peer = args[1];
      if(args.size()>2) port = stoul(args[2]);
    } catch(exception& e){
      handle_invalid_args(e);
      return Message::echo(string("Invalid arguments."));
    }
    /* TODO: give connect command a return message. Now you think you have
     * connected even if you failed.
     */

    core->connect(peer,port);

    // Must return 'const char*', since previous return is that type.

#ifdef TEST
    return Message::echo(string("(<>) Connected to '"+peer+":"+to_string(port)+"'."));
#else
    return Message::echo(string("Connected to '"+peer+":"+to_string(port)+"'."));
#endif

  },
  "connect PEER_IP [PORT]",
  2,3);

  init_command(Commands::CMD_BROADCAST,
               [this](const vector<string>& args){
    string msg;
    try{
      msg = args[1];
    } catch(exception& e){
      handle_invalid_args(e);
      return Message::echo(string("Invalid arguments."));
    }
    core->broadcast_echo(msg);
#ifdef TEST
    return Message::echo(string("done"));
#else
    return Message::echo(string("Broadcast complete."));
#endif
  },
  "broadcast MESSAGE",
  2,2);

  init_command(Commands::CMD_EXIT,
               [this](const vector<string>& args){
    return Message::echo("Exit");
    (void)args;
  },
  "'exit' or 'quit'",
  1,1);

  init_command(Commands::CMD_UPLOAD,
               // Should have args (filename (or file_path later),[tags])
               [this](const vector<string>& args){
    string filename;
    try{
      filename = args[1];
    } catch(exception& e){
      handle_invalid_args(e);
      return Message::echo(string("Invalid arguments."));
    }
    string tags;
    if(args.size() > 2){
      try{
        tags = args[2];
      } catch(exception& e){
        handle_invalid_args(e);
        return Message::echo(string("Invalid arguments."));
      }
    }else{
      tags = filename+":"+filename;
    }

    Id mid;
    if(!core->upload_file(filename, tags, mid)){
      return Message::echo(string("Upload failed."));
    }

    return Message::mid(mid); // mid.to_string() ??

  },
  "upload filename [tags]",
  2,3);

  init_command(Commands::CMD_DOWNLOAD,
               // Should have args (mid)
               [this](const vector<string>& args){

    Id mid;
    try{
      if(!mid.from_string(args[1])){
        return Message::echo(string("Download failed: Invalid mid"));
      }

      Id bid;
      if(!core->req_file(mid,bid)){
        debug("Metahead for mid [%s] doesn't exist",mid);
        return Message::echo(string("Invalid mid: Not found"));
      }

      return Message::bid(bid); // bid.to_string() ??

    } catch(exception& e){
      debug("*** error: %s",e.what());
      handle_invalid_args(e);
      return Message::echo(string("Invalid arguments."));
    }

    return Message::echo(string("Download failed."));

  },
  "download mid",
  2,2);

  init_command(Commands::CMD_SYNCH,
               [this](const vector<string>& args){
    int status = 0;
    int period = SYNC_PERIOD;
    try{
      status = stoi(args[1]);
      if(args.size()>2) period = stoi(args[2]);
    } catch(exception& e){
      handle_invalid_args(e);
      return Message::echo(string("Invalid arguments."));
    }

    if(status != 0){
      core->start_synch(period);
#ifdef TEST
      return Message::echo(string("(<>) Synch started"));
#else
      return Message::echo(string("Synching started with period: " + to_string(period)));
#endif
    }
    else{
      core->stop_synch();
#ifdef TEST
      return Message::echo(string("(<>) Synch stopped"));
    #else
      return Message::echo(string("Synching stopped"));
#endif
    }
  },
  "synch 1|0 [sync_period]",
  2,3);

  init_command(Commands::CMD_ASSEMBLE,
               // Should have args (bid,[file_path="~/Downloads"(eller annat lämpligt val)])
               [this](const vector<string>& args){

    Id bid;
    try{
      if(!bid.from_string(args[1])){
        return Message::echo(string("Assemble failed: Invalid bid"));
      }

      string filename("unnamed_file");
      if(args.size() == 3){
        filename = args[2];
      }
      //TODO: filename checking
      if(!core->get_file(bid,filename)){
        return Message::echo(string("Assembly failed: couldn't find file"));
      }

      return Message::assemble_filename(filename);

    } catch(exception& e){
      debug("*** error: %s",e.what());
      handle_invalid_args(e);
      return Message::echo(string("Invalid arguments."));
    }

    return Message::echo(string("Assembly failed."));

  },
  "assemble bid [filename=\"unnamed_file\"]",
  2,3);

  init_command(Commands::CMD_SEARCH,
               // Should have arg (tag1%tag2)
               [this](const vector<string>& args){

    vector<Id> mids;
    try{
      core->search(args[1],mids);

      vector<Metahead> meta_list;
      for(Id mid : mids){
        Metahead head;
        core->get_metahead(mid,head);
        meta_list.push_back(head);
      }
      return Message::meta_list(meta_list);

    } catch(exception& e){
      debug("*** error: %s",e.what());
      handle_invalid_args(e);
      return Message::echo(string("Invalid arguments."));
    }

    return Message::echo(string("SEARCH failed."));

  },
  "search tag_string",
  2,2);

  init_command(Commands::CMD_MERGE,
               // Should have arg (pid1,pid2)
               [this](const vector<string>& args){

    try{
      peer_id_t pid1 = stoull(args[1]), pid2 = stoull(args[2]);

      if(!core->merge_peers(pid1,pid2)){
        return Message::echo(string("Merge failed"));
      }

#ifdef TEST
      return Message::echo(string("(<>) Merging..."));
#else
      return Message::echo(string("Merging..."));
#endif

    } catch(exception& e){
      debug("*** error: %s",e.what());
      handle_invalid_args(e);
      return Message::echo(string("Invalid arguments."));
    }

  },
  "merge pid1 pid2",
  3,3);

  init_command(Commands::CMD_REQ_PEERS,
               // Should have arg (pid)
               [this](const vector<string>& args){

    try{
      peer_id_t pid1=stoull(args[1]);
      if(!core->make_peer_req(pid1)){
        return Message::echo(string("Request failed."));
      }
      return Message::echo(string("Requesting..."));
    } catch(exception& e){
      debug("*** error: %s",e.what());
      handle_invalid_args(e);
      return Message::echo(string("Invalid arguments."));
    }

  },
  "request pid",
  2,2);
}
