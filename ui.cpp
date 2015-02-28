#include "ui.h"


UI::UI() {
    printf(" ~~~ welcome to darknet ~~~\n");
    thread([this](){get_raw_input();}).detach();
}

void UI::get_raw_input(){
    string raw_input;
    cout << "> ";
    while(getline(cin,raw_input)){
        thread([this,raw_input](){
            try{
                process_raw_input(move(raw_input));
            } catch(std::exception&e){
                debug("*** troll user put? : %s",e.what());
            }
            cout << "\n> ";
        }).detach();
    }
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

    if(cmd_str == "connect") {
        Connect connect_cmd(&cmd_args);
        connect_cmd.execute();

    }else if(cmd_str == "disconnect"){
        Disconnect disconnect_cmd(&cmd_args);
        disconnect_cmd.execute();

    }else if(cmd_str == "fetch"){
        Fetch fetch_cmd(&cmd_args);
        fetch_cmd.execute();

    }else if(cmd_str == "metalist"){
        Metalist metalist_cmd(&cmd_args);
        metalist_cmd.execute();

    }else if(cmd_str == "broadcast"){
        Broadcast broadcast_cmd(&cmd_args);
        broadcast_cmd.execute();

    }else if(cmd_str == "encrypt"){
        Encrypt encrypt_cmd(&cmd_args);
        encrypt_cmd.execute();

    }else if(cmd_str == "decrypt"){
        Decrypt decrypt_cmd(&cmd_args);
        decrypt_cmd.execute();

    }else if(cmd_str == "help" || cmd_str == "?"){
        Help help_cmd(&cmd_args);
        help_cmd.execute();

    }else if(cmd_str == "quit" || cmd_str == "exit"){
        Quit quit_cmd(&cmd_args);
        quit_cmd.execute();

    }else{
        cout << cmd_args[0] << ": Command not found" << endl;
    }

}
