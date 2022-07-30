#ifndef SERVER_USER_SERVER_H
#define SERVER_USER_SERVER_H

#include <thread>
#include <unistd.h>

class User_server {
public:
    User_server(int _id, int _client_socket);
    ~User_server();

    int id;
    std::thread* client_thread;
    int client_socket;
    std::string name;
    std::string username;
};

#endif //SERVER_USER_SERVER_H
