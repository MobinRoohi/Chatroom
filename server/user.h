#ifndef SERVER_USER_H
#define SERVER_USER_H

#include "user_server.h"
#include <map>

class User {
public:
    User(std::string name, std::string username, std::string password, User_server* _user_server=0);

    User_server* user_server;
    std::string username;
    std::string name;
    std::string password;
};

#endif //SERVER_USER_H
