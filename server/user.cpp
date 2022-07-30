#include "user.h"

User::User(std::string _name, std::string _username, std::string _password, User_server* _user_server):
        user_server(_user_server), password(_password), username(_username), name(_name) {}