#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include <iostream>
#include <mutex>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include "user_server.h"
#include "user.h"
#include "user_file.h"
#include "group.h"
#include "databases.h"


class Server {
public:
    Server();
    void start_listening();
    void start_accepting();
    void close_connection();
    void end_connection(int id);
    static void handle_client(Server* server, User_server* user_server);
    void multi_print(std::string str);
    void add_client(User_server* client);
    void send_message(int socket, std::string message);
    void do_for_user(User* user, std::string message);
    bool try_add_or_get_user(User_server* user_server, std::string username, std::string name, std::string password);
    void send_pv(User_server* sender, User_server* client, std::string mes);
    bool login_client(User_server* user_server);
    void check_user(std::string name, bool connected=false);
    void get_info_from_file();
    void delete_users();
    void send_group(User_server* sender, Group* dest_group, string message);
    void broadcast(string, User_server*, map<string, User*>);
    void check_group(string name, int i);
    void add_group(string name, User* owner);
    void invite_group(string group_name, string invitee_name, string inviter);
    void show_groups(User* user);
    void show_blocked(User* user);
    void unblock(string blocker, string blocked);
    void block(string blocker, string blocked);
    void check_block(string blocker, string blocked);
    void show_history_pv(User *a_user, User *b_user);
    void show_history_group(User *user, Group *group);
    ~Server();

    int unique_id;
    int MAX_LEN;
    int server_socket;
    std::mutex print_mtx, clients_mtx;
    std::map<int, User_server*> clients;
    std::map<std::string, User*> users;
    std::map<std::string, Group*> groups;
    std::multimap<std::string, std::string> blocks;
    Database * DB;

};


#endif //SERVER_SERVER_H
