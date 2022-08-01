#include "databases.h"
#include <iostream>
using namespace std;

Database::Database() {
    users = "users_DB.txt";
    groups = "groups_DB.txt";
    user_group = "user_group_DB.txt";
    pv_msg = "pv_msg_DB.txt";
    group_msg = "group_msg_DB.txt";
    buffer_group = "buffer_group_DB.txt";
    buffer_pv = "buffer_pv_DB.txt";
    blocks = "blocks_DB.txt";

    field_count["users"] = 2;        // username - password
    field_count["groups"] = 2;       // name - owner
    field_count["user_group"] = 2;   // username - group name
    field_count["pv_msg"] = 3;       // sender - receiver - msg
    field_count["group_msg"] = 3;    // sender - group name - msg
    field_count["buffer_group"] = 3; // sender - receiver - msg
    field_count["buffer_pv"] = 3;    // sender - group - msg
    field_count["blocks"] = 2;       // blocker - blocked


}

void Database::insert_database(vector<string> fields, const char *address) {
//    lock_guard<mutex> guard(pr);
    ifstream fileread(address, ios::in);
    const char *address2 = "a.text";
    ofstream filewrite(address2, ios::out);
    string cc;
    while(getline(fileread, cc)) {
       filewrite << cc << endl;
    }
    for (int i = 0; i < fields.size(); i++) {
        if (i == 0) {
            filewrite << "#" << fields[i];
            continue;
        }
        filewrite << " #" << fields[i];
    }
    filewrite << endl;
    remove(address);
    rename(address2, address);
    filewrite.close();
    fileread.close();
}

void Database::delete_from_database(int column, string target, string address) {}

void Database::read_database(map<int, string> target, string address) {}

