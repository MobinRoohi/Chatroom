#include "user_file.h"

User_file::User_file(string _addr, string _wrapper):
        addr(_addr), wrapper(_wrapper) {}

void User_file::add_user(User* user) {
    lock_guard<mutex> guard(write_mtx);
    file.open(addr, ios::out);
    file << user->username << wrapper << user->password;
    file.close();
}

vector<User*>* User_file::get_users() {
    vector<User*>* users = new vector<User*>();
    lock_guard<mutex> guard(write_mtx);
    file.open(addr, ios::in);
    string line;
    vector<string>* messages;
    while(!file.eof()) {
        getline(file,line);
        if (!line.length())
            continue;
        messages = split_message(line, wrapper, 0);
//        users->push_back(new User(messages->at(0), messages->at(1)));
    }
    file.close();
    return users;
}