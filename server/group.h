#ifndef SERVER_GROUP_H
#define SERVER_GROUP_H

#include "user.h"
#include <vector>
using namespace std;

class Group {
public:
    std::string name;
    User* ownerUser;
    map<string, User*> group_members;
    Group(std::string, User*);
    void try_add_group(User* invitee);
    void member_exits(string user_name);
};

#endif //SERVER_GROUP_H
