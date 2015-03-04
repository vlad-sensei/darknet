#include "command.h"


vector<char*> command_keys;

/*
 *      Connect
 *
 * Connect to known node.
 */
bool Connect::verify_args(){
  // TODO: Better check
  if(args.size()==2 || args.size()==3){
    return true;
  }
  debug("Invalid command: Wrong argument count");
  return false;
}

bool Connect::execute(){
  if(verify_args()){

    uint16_t port = LISTEN_PORT;
    if(args.size() == 3){
      port = strtol((args.at(2)).c_str(),NULL,10);
    }
    string peer = args.at(1);
    core->connect(peer,port);
    return true;
  }else{
    return false;
  }
}

string Connect::help_text(){
  return "Help to connect";
}
// END Connect

/*
 *      Disconnect
 *
 * Disconnect from connected node.
 */
bool Disconnect::verify_args(){
  // TODO: Better check
  if(args.size()>1){
    return true;
  }
  debug("Invalid command: To few arguments");
  return false;
}

bool Disconnect::execute(){
  if(verify_args()){
    debug(help_text().c_str());
    return true;
  }else{
    return false;
  }
}

string Disconnect::help_text(){
  return "Help to disconnect";
}
// END Disconnect

/*
 *      Broadcast
 *
 * Broadcast a (string-)message (to all open connections?)
 */
bool Broadcast::verify_args(){
  // TODO: Better check
  if(args.size()==2){
    return true;
  }
  debug("Invalid command: Wrong argument count");
  return false;
}

bool Broadcast::execute(){
  if(verify_args()){
    core->broadcast_echo(args.at(1));
    return true;
  }else{
    return false;
  }
}

string Broadcast::help_text(){
  return "Help to broadcast";
}
// END Broadcast


/*
 *      Fetch
 *
 * Fetch a payload, named in metafile
 */
bool Fetch::verify_args(){
  // TODO: Better check
  if(args.size()>1){
    return true;
  }
  debug("Invalid command: To few arguments");
  return false;
}

bool Fetch::execute(){
  if(verify_args()){
    debug(help_text().c_str());
    return true;
  }else{
    return false;
  }
}

string Fetch::help_text(){
  return "Help to fetch";
}
// END Fetch

/*
 *      Metalist
 *
 * Displays metafiles.
 */
bool Metalist::verify_args(){
  return true;
}

bool Metalist::execute(){
  if(verify_args()){
    debug(help_text().c_str());
    return true;
  }else{
    return false;
  }
}

string Metalist::help_text(){
  return "Help to metalist";
}
// END Metalist


/*
 *      Encrypt
 *
 * Encrpyt a file
 */
bool Encrypt::verify_args(){
  // TODO: Better check
  if(args.size()>1){
    return true;
  }
  debug("Invalid command: To few arguments");
  return false;
}

bool Encrypt::execute(){
  if(verify_args()){
    debug(help_text().c_str());
    return true;
  }else{
    return false;
  }
}

string Encrypt::help_text(){
  return "Help to encrypt";
}
// END Encrypt

/*
 *      Decrypt
 *
 * Decrypt a file
 */
bool Decrypt::verify_args(){
  // TODO: Better check
  if(args.size()>1){
    return true;
  }
  debug("Invalid command: To few arguments");
  return false;
}

bool Decrypt::execute(){
  if(verify_args()){
    debug(help_text().c_str());
    return true;
  }else{
    return false;
  }
}

string Decrypt::help_text(){
  return "Help to decrypt";
}
// END Decrypt

/*
 *      Help
 *
 * Show available commands and other useful info.
 */
bool Help::verify_args(){
  if((args.size() == 1)
     || (args.size()==2
         && find(command_keys.begin(), command_keys.end(), args.at(1))
         != command_keys.end())){
    return true;
  }
  debug("Invalid command Help: To few arguments");
  return false;
}

bool Help::execute(){
//  cout << help_text() << endl;
  if(verify_args()){
    if(args.size() == 1){
      // Show available commands
      cout << help_text() << endl;
    }else{ //Has verified to have legal command argument as second arg
      shared_ptr<Command> cmd = command_map[args.at(1)];
      cout << cmd->help_text() << endl;
    }
    return true;

  }else{
    return false;
  }
}

string Help::help_text(){

  static string help_str = "";

  if(help_str.empty()){
    help_str = help_str
        + "Usage:\n"
        +"'help' - show this help\n"
        + "'help <command> - show help about specific command\n"
        + "\n"
        + "Available commands:"; //Left out last '\n'
    for(auto str: command_keys){
      help_str = help_str + "\n" + str;
    }
  }

  return help_str;
}
//END Help

/*
 *      Quit
 *
 * Exit the program.
 */
bool Quit::execute(){  
  cout << "Exiting program..." << endl;
  exit(0);
  return true;
}

string Quit::help_text(){
  return "Help to Quit";
}

//END Help

