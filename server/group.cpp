#include "group.h"

Group::Group(std::string name, User* owner) {
    this->name = name;
    this->ownerUser = owner;
    this->group_members[owner->username] = owner;
}

void Group::try_add_group(User* invitee) {
    if (group_members.find(invitee->username) == group_members.end()) {
        group_members[invitee->username] = invitee;
    }
    else {
        throw "Member already exists";
    }
}

void Group::member_exits(string user_name) {
    if (group_members.find((user_name)) == group_members.end()) {
        throw "You are not a member of the given group";
    }

}