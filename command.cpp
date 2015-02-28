#include "command.h"

/*
 *      Connect
 *
 * Connect to known node.
 */
bool Connect::verify_args(){
    // TODO: Better check
    if(args->size()==2){
        return true;
    }
    debug("Invalid command: Wrong argument count");
    return false;
}

bool Connect::execute(){
    if(verify_args()){
//        uint16_t port = args[2];
        uint16_t port = LISTEN_PORT;
        string peer = (*args).at(1);
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
    if(args->size()>1){
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
    if(args->size()==2){
        return true;
    }
    debug("Invalid command: Wrong argument count");
    return false;
}

bool Broadcast::execute(){
    if(verify_args()){
        core->broadcast_echo((*args).at(1));
//        debug(help_text().c_str());
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
    if(args->size()>1){
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
 *      Encrypt
 *
 * Encrpyt a file
 */
bool Encrypt::verify_args(){
    // TODO: Better check
    if(args->size()>1){
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
    if(args->size()>1){
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
bool Help::execute(){
    debug(help_text().c_str());
    return true;
}

string Help::help_text(){
    // Have help text as variable/constant? Or how?
    return "Available commands:\n connect\n disconnect\n broadcast\n fetch\n encrypt\n decrypt\n help";
}
//END Help

/*
 *      Quit
 *
 * Exit the program.
 */
bool Quit::execute(){
//    debug(help_text().c_str());
    debug("Exiting program...");
    exit(0);
    return true;
}

string Quit::help_text(){
    return "Help to Quit";
}
//END Help

