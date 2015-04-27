#include "ui.h"

#include <sstream>
#include <iostream>
#include <fstream>
#include <iterator>

#include "core.h"

UI::UI() {
  init_commands();
}


void UI::run(){
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

string UI::process_text_input(const string& text_input){
  istringstream ss(text_input);
  //istream_iterator<string> begin(ss), end;
  //vector<string> args(begin, end;
  istream_iterator<string> begin(ss), end;
  vector<string> cmd_args(begin, end);
  if(cmd_args.empty()){
    return "";
  }
  Commands cmd_enum = command_map[cmd_args[0]];

  r_lock(commands_mtx);
  if(!data.command_exists(cmd_enum)){
    //safe_printf("No such command: %s\n", cmd_args[0]);
    return "No such command: "+ cmd_args[0];
  }
  return data.commands[cmd_enum]->exec(cmd_args);
}

string UI::Command::exec(const vector<string> &args){
  if(args.size()<minargc_||args.size()>maxargc_){
    return "Wrong argument count\n"+help();
  }
  try{
    return execute_(args);
  } catch(exception&e){
    safe_printf(" *** error processing command : %s\n",e.what());
  }
  return "";
}

//TODO: make sure there are no copies
void UI::init_command(const Commands cmd_enum, const cmd_lambda_t &execute, const string &help_text, const unsigned &minargc, const unsigned &maxargc){
  Command_ptr cmd = Command_ptr(new Command(execute, help_text, minargc, maxargc));
  w_lock w(commands_mtx);
  data.commands[cmd_enum] = cmd;
  // for(const string& key_word : key_words) data.commands[key_word]=cmd;
}

//TODO: check if that this does not lead to any race condition when "this" (UI/Core) is destroyed
void UI::init_commands(){
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
      return "Invalid arguments.";
    }
    /* TODO: give connect command a return message. Now you think you have
     * connected even if you failed.
     */

    core->connect(peer,port);

    // Must return 'const char*', since previous return is that type.
    string ret_str = "Connected to '"+peer+":"+to_string(port)+"'.";
    return ret_str.c_str();
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
      return "Invalid arguments.";
    }
    core->broadcast_echo(msg);
    return "Broadcast complete.";
  },
  "broadcast MESSAGE",
  2,2);

  init_command(Commands::CMD_EXIT,
               [this](const vector<string>& args){
    safe_printf("Exiting darknet...");
    exit(0);
    return "";
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
      return "Invalid arguments.";
    }
    string tags;
    if(args.size() > 2){
      try{
        tags = args[2];
      } catch(exception& e){
        handle_invalid_args(e);
        return "Invalid arguments.";
      }
    }else{
      tags = filename+":"+filename;
    }

    Id mid;
    if(!core->upload_file(filename, tags, mid)) return "Upload failed.";
    string ret_str = "Upload successful: mid[" + mid.to_string() +"]";
    return ret_str.c_str();
  },
  "upload filename [tags]",
  2,3);

  init_command(Commands::CMD_DOWNLOAD,
               // Should have args (mid)
               [this](const vector<string>& args){

    Id mid;
    try{
      //mid = to_Id(args[1]);

      debug("MID:[%s]",mid);
      if(!mid.from_string(args[1])) return "Download failed: Invalid mid";

      Id bid;
      if(!core->req_file(mid,bid)){
        debug("Metahead for mid [%s] doesn't exist",mid);
        return "Invalid mid: Not found";
      }
      string ret_str = "Download succeded. File bid:["+bid.to_string()+"]";
      return ret_str.c_str();

    } catch(exception& e){
      debug("*** error: %s",e.what());
      handle_invalid_args(e);
      return "Invalid arguments.";
    }

    return "download failed.";

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
      return "Invalid arguments.";
    }

    if(status){
      core->start_synch(period);
      return string("synching started with period: " + to_string(period)).c_str();
    }
    else{
      core->stop_synch();
      return "syncing stopped";
    }
  },
  "synch 1|0 [sync_period]",
  2,3);

  init_command(Commands::CMD_ASSEMBLE,
               // Should have args (bid,[file_path="~/Downloads"(eller annat lämpligt val)])
               [this](const vector<string>& args){

    Id bid;
    try{
      if(!bid.from_string(args[1])) return "Assemble failed: Invalid bid";
      debug("BID:[%s]",bid);
      string filename("unnamed_file");
      if(args.size() == 3){
        filename = args[2];
      }
      //TODO: filename checking
      if(!core->get_file(bid,filename)) return "Assembly failed: couldn't find file";

      return "Assembly complete!";
    } catch(exception& e){
      debug("*** error: %s",e.what());
      handle_invalid_args(e);
      return "Invalid arguments.";
    }

    return "Assembly failed.";

  },
  "assemble bid [filename=\"unnamed_file\"]",
  2,3);

  init_command(Commands::CMD_SEARCH,
               // Should have arg (tag1%tag2)
               [this](const vector<string>& args){

    vector<Id> mids;
    try{
      core->search(args[1],mids);
      for(Id mid:mids){
        Metahead head;
        core->get_metahead(mid,head);
        debug("[mid %s]\n [tags %s]\n [bid %s]\n",head.mid, head.tags,head.bid);
        return "SEARCHING ...";
      }
    } catch(exception& e){
      debug("*** error: %s",e.what());
      handle_invalid_args(e);
      return "Invalid arguments.";
    }

    return "SEARCH failed.";

  },
  "assemble bid [filename=\"unnamed_file\"]",
  2,2);

  init_command(Commands::CMD_MERGE,
               // Should have arg (tag1%tag2)
               [this](const vector<string>& args){

    try{
      peer_id_t pid1=stoull(args[1]),pid2=stoull(args[2]);
      if(!core->merge_peers(pid1,pid2)) return "merge faild";
      return "merging...";
    } catch(exception& e){
      debug("*** error: %s",e.what());
      handle_invalid_args(e);
      return "Invalid arguments.";
    }



  },
  "assemble bid [filename=\"unnamed_file\"]",
  3,3);

  init_command(Commands::CMD_REQ_PEERS,
               // Should have arg (tag1%tag2)
               [this](const vector<string>& args){

    try{
      peer_id_t pid1=stoull(args[1]);
      if(!core->make_peer_req(pid1)) return "reqest faild";
      return "reqesting...";
    } catch(exception& e){
      debug("*** error: %s",e.what());
      handle_invalid_args(e);
      return "Invalid arguments.";
    }



  },
  "assemble bid [filename=\"unnamed_file\"]",
  2,2);




}
