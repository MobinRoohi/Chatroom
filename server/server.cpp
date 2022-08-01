#include "server.h"

Server::Server() {
    unique_id = 0;
    MAX_LEN = 150;
    clients.clear();
    DB = new Database;
}

void Server::start_listening() {
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation failed!");
        exit(-1);
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(10000);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if ((::bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address))) == -1) {
        perror("binding failed!");
        exit(-1);
    }

    if ((listen(server_socket, 5)) == -1) {
        perror("listening failed!");
        exit(-1);
    }

    multi_print("... Chat Room Is Listening ...");
}

void Server::start_accepting() {
    struct sockaddr_in client_address;
    int client_socket;
    unsigned int len = sizeof(sockaddr_in);

    while (true) {
        if ((client_socket = accept(server_socket, (struct sockaddr *)&client_address, &len)) == -1) {
            perror("accepting failed!");
            exit(-1);
        }
        unique_id++;
        User_server* user_server = new User_server(unique_id, client_socket);
        std::thread* client_thread = new std::thread(handle_client, this, user_server);
        user_server->client_thread = client_thread;
        add_client(user_server);
    }
}

void Server::multi_print(std::string str) {
    std::lock_guard<std::mutex> guard(print_mtx);
    std::cout << str << std::endl;
}

void Server::add_client(User_server* user_server) {
    std::lock_guard<std::mutex> guard(clients_mtx); //?
    clients[user_server->id] = user_server;
}

void Server::handle_client(Server* server, User_server* user_server) {
    if (server->login_client(user_server)) {
        char message[server->MAX_LEN];
        int bytes_received;
        while (true) {
            bytes_received = recv(user_server->client_socket, message, sizeof(message), 0);
            if (bytes_received <= 0)
                break;
            server->multi_print(std::string(user_server->username) + " : " + std::string(message));
            server->do_for_user(server->users[user_server->username], message);
        }
        server->users[user_server->username]->user_server = 0;
    }
    server->end_connection(user_server->id);
}

bool Server::login_client(User_server* user_server) {
    while(true) {
        char name[MAX_LEN], password[MAX_LEN], username[MAX_LEN];
        int bytes_received = recv(user_server->client_socket, username, sizeof(username), 0);
        if(bytes_received <= 0)
            return false;
        bytes_received = recv(user_server->client_socket, password, sizeof(password), 0);
        if(bytes_received <= 0)
            return false;
        bytes_received = recv(user_server->client_socket, name, sizeof(name), 0);
        if(bytes_received <= 0)
            return false;
        if (!try_add_or_get_user(user_server, username, name, password)) {
            std::string error_message = "Server | Invalid Credentials For User " + std::string(username);
            send_message(user_server->client_socket, error_message);
            multi_print(error_message);
            continue;
        }
        // We give the user server a name after logging in.
        user_server->username = username;
        user_server->name = name;
        std::string welcome_message = "Server | Welcome " + std::string(name) + " (@" + std::string(username) + ")";
        send_message(user_server->client_socket, welcome_message);
        multi_print(welcome_message);
        return true;
    }
}

bool Server::try_add_or_get_user(User_server* user_server, std::string username, std::string name, std::string password) {
    if (users.find(username) != users.end()) {
        if (users[username]->password == password) {
            users[username]->user_server = user_server;
            users[username]->name = name;
        }
        else
            return false;
    }
    else {
        users[username] = new User(name, username, password, user_server);
//        user_file->add_user(users[username]);
        vector<string> a = {username, password, name};
        DB->insert_database(a, "users_DB.txt");
    }
    return true;
}

void Server::send_message(int client_socket, string message) {
    send(client_socket, &message[0], MAX_LEN, 0);
}

void Server::do_for_user(User* user, string message) {
    if (message.size() == 0) {
        throw "";
    }
    vector<string>* message_splitted = 0;
    try {
        message_splitted = split_message(message, " #");
        check_message_size(message_splitted);
        if (message_splitted->at(0) == "pv") {
            check_message_size(message_splitted, 3);
            check_user(message_splitted->at(1), true);
            check_block(message_splitted->at(1), user->username);
            send_pv(user->user_server, users[message_splitted->at(1)]->user_server, message_splitted->at(2));
        } else
        if (message_splitted->at(0) == "group") {
            check_message_size(message_splitted, 3);
            check_group(message_splitted->at(1), 1);
            groups[message_splitted->at(1)]->member_exits(user->username);
            send_group(user->user_server, groups[message_splitted->at(1)], message_splitted->at(2));
        } else
        if (message_splitted->at(0) == "creategroup") {
            check_message_size(message_splitted, 2);
            add_group(message_splitted->at(1), user);
            send_message(user->user_server->client_socket, "Server | Group | You created the group : " + message_splitted->at(1));
            broadcast(user->user_server->name + " (@" + user->username + ")" + " created the group", user->user_server, groups[message_splitted->at(1)]->group_members);
        } else
        if (message_splitted->at(0) == "invitegroup") {
            check_message_size(message_splitted, 3);
            check_block(message_splitted->at(2), user->username);
            groups[message_splitted->at(1)]->member_exits(user->username);
            invite_group(message_splitted->at(1), message_splitted->at(2), user->username);
        } else
        if (message_splitted->at(0) == "showgroups") {
            check_message_size(message_splitted, 1);
            show_groups(user);
        } else
        if (message_splitted->at(0) == "blockedlist") {
            check_message_size(message_splitted, 1);
            show_blocked(user);
        } else
        if (message_splitted->at(0) == "block") {
            check_message_size(message_splitted, 2);
            check_user(message_splitted->at(1));
            block(user->username, message_splitted->at(1));
        } else
        if (message_splitted->at(0) == "unblock") {
            check_message_size(message_splitted, 2);
            check_user(message_splitted->at(1));
            unblock(user->username, message_splitted->at(1));
        } else
        if (message_splitted->at(0) == "history") {
            check_message_size(message_splitted, 3);
            if (message_splitted->at(1) == "pv") {
                check_user(message_splitted->at(2));
                show_history_pv(user, users[message_splitted->at(2)]);
            } else
            if (message_splitted->at(1) == "group") {
                check_group(message_splitted->at(2), 1);
                show_history_group(user, groups[message_splitted->at(2)]);
            }
        }
        else
            throw "The command is not executable";
    }
    catch(const char* msg) {
        send_message(user->user_server->client_socket, "Server | There is a problem ( " + string(msg) + " )");
    }
    if (message_splitted) {
        message_splitted->clear();
        delete message_splitted;
    }
}

void Server::show_history_pv(User *a_user, User *b_user) {
    vector<string> history = {"a"};
    for (int i = 1; true; i++) {
        history = DB->extract_database(i, "pv_msg_DB.txt");
        if (history.size() == 0) {
            history = {"a"};
            break;
        }
        if (history[1] == a_user->username && history[2] == b_user->username) {
            send_message(a_user->user_server->client_socket, "PV | History | You : " + history[3]);
            continue;
        }
        if (history[1] == b_user->username && history[2] == a_user->username) {
            send_message(a_user->user_server->client_socket, "PV | History | " + users[history[1]]->name + " (@" + history[1] + ") : " + history[3]);
            continue;
        }
    }
}

void Server::show_history_group(User *user, Group *group) {
    vector<string> history = {"a"};
    for (int i = 1; true; i++) {
        history = DB->extract_database(i, "group_msg_DB.txt");
        if (history.size() == 0) {
            history = {"a"};
            break;
        }
        if (history[2] == group->name && history[1] == user->username) {
            send_message(user->user_server->client_socket, "Groups | History | " + history[2] + " | You : " + history[3]);
            continue;
        }
        if (history[2] == group->name && history[1] != user->username) {
            send_message(user->user_server->client_socket, "Groups | History | " + history[2] + " | " + users[history[1]]->name + " (@" + history[1] + ") : " + history[3]);
            continue;
        }
    }
}

void Server::unblock(string blocker, string blocked) {
    for (auto a : blocks) {
        if (a.first == blocker && a.second == blocked) {
            auto i = blocks.begin();
            for (i = blocks.begin(); i->first != blocker || i->second != blocked; i++) {}
            blocks.erase(i);
            send_message(users[blocker]->user_server->client_socket, "Server | You have successfully unblocked " + users[blocked]->name + " (@" + blocked + ")");
            map<int, string> a; a[1] = blocker; a[2] = blocked;
            DB->delete_from_database(a, "blocks_DB.txt");
            return;
        }
    }
    throw "The given user is not blocked by you";
}

void Server::check_block(string blocker, string blocked) {
    bool blocked_ = false;
    for (auto a : blocks) {
        if (a.first == blocker && a.second == blocked) {
            blocked_ = true;
            break;
        }
    }
    if (blocked_) {
        throw "The user has blocked you";
    }

}

void Server::block(string blocker, string blocked) {
//    bool exists = false;
    for (auto a : blocks) {
        if (a.first == blocker && a.second == blocked) {
            throw "You have already blocked this user";
        }
    }
    blocks.insert(pair<string, string>(blocker, blocked));
    send_message(users[blocker]->user_server->client_socket, "Server | You have successfully blocked " + users[blocked]->name + " (@" + blocked + ")");
    vector<string> a {blocker, blocked};
    DB->insert_database(a, "blocks_DB.txt");
}

void Server::show_blocked(User* user) {
    bool exists = false;
    int ind = 1;
    string msg;
    for (auto a : blocks) {
        if (a.first == user->username) {
            if (!exists)
                send_message(user->user_server->client_socket, "Server | Here is a list of the users you have blocked :");
            exists = true;
            msg = to_string(ind++) + ". " + users[a.second]->name + " (@" + a.second + ")";
            send_message(user->user_server->client_socket, msg);
        }
    }
    if (!exists) {
        send_message(user->user_server->client_socket, "Server | You have not blocked anyone");
    }

}

void Server::show_groups(User* user) {
    int count = 0;
    vector<string> groups_identified;
    string msg;
    for (auto a : groups) {
        if (a.second->group_members.find(user->username) != a.second->group_members.end()) {
            groups_identified.push_back(a.first);
            count++;
        }
    }
    if (count == 0) {
        send_message(user->user_server->client_socket, "Server | You have not joined any groups");
    }
    else {
        send_message(user->user_server->client_socket, "Server | Here is a list of the groups you have joined :");
        for (int i = 0; i < groups_identified.size(); i++) {
            msg = to_string(i + 1) + ". " + groups_identified[i];
            send_message(user->user_server->client_socket, msg);
        }
    }
}

void Server::send_pv(User_server* sender, User_server* client, string message) {
    send_message(client->client_socket, "PV | " + sender->name + " (@" + sender->username + ")" +  " : " + message);
    send_message(sender->client_socket, "Server | ✓✓ You To " + client->name + " (@" + client->username + ")" + " : " + message);
    vector<string> a {sender->username, client->username, message};
    cout << message << endl;
    DB->insert_database(a, "pv_msg_DB.txt");
//    send_message(sender->client_socket, "✓✓");

}

void Server::check_user(string username, bool connected) {
    if (users.find(username) == users.end())
        throw "This user does not exist";
    if (connected && !users[username]->user_server)
        throw "This user is not connected";
}

void Server::send_group(User_server* sender, Group* dest_group, string message) {
    broadcast("Groups | " + dest_group->name + " | " + sender->name + " (@" + sender->username + ")" + " : " + message, sender, dest_group->group_members); // MAYBE MORE IN ARGUMENTS!
    send_message(sender->client_socket, "Groups | " + dest_group->name + " | ✓✓ You : " + message);
    vector<string> a {sender->username, dest_group->name, message};
    DB->insert_database(a, "group_msg_DB.txt");

}

void Server::broadcast(string msg, User_server* sender_server, map<string, User *> users) {
    for (auto &user : users) {
        if (user.first != sender_server->username && user.second->user_server) {
            send_message(user.second->user_server->client_socket, msg);
        }
    }
}

void Server::check_group(string name, int sig) {
    if (groups.find(name) == groups.end()) {
        if (sig == 1) {
            throw "This group does not exits!";
        }
    }
    else {
        if (sig == 0) {
            throw "Group name is not available!";
        }
    }
}

void Server::add_group(string name, User* owner) {
    check_group(name, 0);
    groups[name] = new Group(name, owner);
    vector<string> a = {name, owner->username}; vector<string> b {owner->username, name};
    DB->insert_database(a, "groups_DB.txt");
    DB->insert_database(b, "user_group_DB.txt");
    // blah blah blah
}

void Server::invite_group(string group_name, string invitee_name, string inviter) {
    check_group(group_name, 1);
    check_user(invitee_name);
    groups[group_name]->try_add_group(users[invitee_name]);
    if (users[inviter]) {
        vector<string> a {invitee_name, group_name};
        DB->insert_database(a, "user_group_DB.txt");
        send_message(users[inviter]->user_server->client_socket, "Server | You invited " + invitee_name + " to the group : " + group_name);
        broadcast("Groups | " + group_name + " | "+ inviter + " (@" + users[inviter]->username + ")" + " invited " + invitee_name + " (@" + users[invitee_name]->username + ")" + " to the group", users[inviter]->user_server, groups[group_name]->group_members);
    }
}

void Server::end_connection(int id) {
    lock_guard<mutex> guard(clients_mtx);
    if (clients[id])
        delete clients[id];
}

void Server::get_info_from_file() {
    vector<string> a {"a"};
    for (int i = 1; true; i++) {
        a = DB->extract_database(i, "users_DB.txt");
        if (a.size() == 0) {
            a = {"a"};
            break;
        }
        users[a[1]] = new User(a[3], a[1], a[2]);
    }
    for (int i = 1; true; i++) {
        a = DB->extract_database(i, "groups_DB.txt");
        if (a.size() == 0) {
            a = {"a"};
            break;
        }
        groups[a[1]] = new Group(a[1], users[a[2]]);
    }
    for (int i = 1; true; i++) {
        a = DB->extract_database(i, "user_group_DB.txt");
        if (a.size() == 0) {
            a = {"a"};
            break;
        }
        groups[a[2]]->group_members[a[1]] = users[a[1]];
    }
    for (int i = 1; true; i++) {
        a = DB->extract_database(i, "blocks_DB.txt");
        if (a.size() == 0) {
            a = {"a"};
            break;
        }
        blocks.insert(pair<string, string>(a[1], a[2]));
    }
}

void Server::delete_users()
{
    for (auto & u : clients) {
        delete u.second;
    }
    clients.clear();
    for (auto & u : users) {
        delete u.second;
    }
    users.clear();
}

void Server::close_connection() {
    close(server_socket);
}

Server::~Server() {
//    delete user_file;
    delete_users();
    close_connection();
}






