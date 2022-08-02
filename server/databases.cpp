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

    field_count["users_DB.txt"] = 2;        // username - password
    field_count["groups_DB.txt"] = 2;       // name - owner
    field_count["user_group_DB.txt"] = 2;   // username - group name
    field_count["pv_msg_DB.txt"] = 3;       // sender - receiver - msg
    field_count["group_msg_DB.txt"] = 3;    // sender - group name - msg
    field_count["buffer_group_DB.txt"] = 3; // sender - receiver - msg
    field_count["buffer_pv_DB.txt"] = 3;    // sender - group - msg
    field_count["blocks_DB.txt"] = 2;       // blocker - blocked
}

void Database::insert_database(vector<string> fields, const char *address) {
    lock_guard<mutex> guard(pr);
    ifstream fileread(address, ios::in);
    const char *address2 = "a.text";
    ofstream filewrite(address2, ios::out);
    string cc;
    int count = 1;
    while(getline(fileread, cc)) {
       filewrite << cc << endl;
       count++;
    }
    filewrite << "#" + to_string(count);
    for (int i = 0; i < fields.size(); i++) {
        filewrite << " #" << fields[i];
    }
    filewrite << endl;
    remove(address);
    rename(address2, address);
    filewrite.close();
    fileread.close();
}

void Database::cut_string(string ms, vector<string> &ans) {
    string aa = "";
    for (int i = 1; i < ms.size(); i++) {
        if (ms[i] == '#') {
            ans.push_back(aa.substr(0, aa.size() - 1));
            aa = "";
            continue;
        }
        aa += ms[i];
    }
    ans.push_back(aa);
}

void Database::delete_from_database(map<int, string> target, const char *address) {
    lock_guard<mutex> guard(pr);
    ifstream fileread(address, ios::in);
    const char *address2 = "a.text";
    ofstream filewrite(address2, ios::out);
    string cc;
    vector<string> a;
    vector<string> ans;
    bool once = false;
    bool delete_ = true;
    int count = 1;
    while (getline(fileread, cc)) {
        cut_string(cc, ans);
        for (int i = 0; i < ans.size(); i++) {
            if (target.find(i) != target.end()) {
                if (target[i] != ans[i]) {
                    for (int i = 0; i < ans.size(); i++) {
                        if (i == 0) {
                            filewrite << "#" << count;
                            continue;
                        }
                        filewrite << " #" << ans[i];
                    }
                    filewrite << endl;
                    count++;
                    break;
                }
            }
        }
        ans.clear();
    }
    remove(address);
    rename(address2, address);
    filewrite.close();
    fileread.close();
}

vector<string> Database::extract_database(int target, const char *address) {
    lock_guard<mutex> guard(pr);
    ifstream fileread(address, ios::in);
    string cc;
    vector<string> ans;
    int count = 1;
    while (getline(fileread, cc)) {
        if (count == target) {
            cut_string(cc, ans);
            return ans;
        }
        count++;
    }
    return ans;
}

