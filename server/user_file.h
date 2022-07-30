#ifndef SERVER_USER_FILE_H
#define SERVER_USER_FILE_H

#include <iostream>
#include "user.h"
#include <mutex>
#include "helper.h"
#include <vector>
#include <fstream>

using namespace std;

class User_file
{
public:
    User_file(string _addr, string _wrapper);
    void add_user(User* user);
    vector<User*>* get_users();

    string addr;
    fstream file;
    string wrapper;
    mutex write_mtx;
};

#endif //SERVER_USER_FILE_H
