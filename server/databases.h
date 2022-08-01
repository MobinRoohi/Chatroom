#ifndef SERVER_DATABASES_H
#define SERVER_DATABASES_H

#include "user.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <mutex>

using namespace std;

class Database {
public:
    string users;
    string groups;
    string user_group;
    string pv_msg;
    string group_msg;
    string buffer_group;
    string buffer_pv;
    string blocks;
    map<string, int> field_count;
    mutex pr;
//    fstream users_object;
//    fstream groups_object;
//    fstream user_group_object;
//    fstream pv_msg_object;
//    fstream group_msg_object;
//    fstream buffer_object;
//    fstream blocks_object;
    Database();
    void insert_database(vector<string> fields, const char *address);
    void delete_from_database(int column, string target, string address);
    void read_database(map<int, string> target, string address);
//    void users_read();
//    void groups_read();
//    void user_group_read();
//    void pv_msg_read();
//    void pv_msg_read();
};


#endif //SERVER_DATABASES_H
