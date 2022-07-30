#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <mutex>
#include <unistd.h>
#include <thread>
#include <netinet/in.h>

using namespace std;

class Client {
public:
    Client();
    void start_connecting();
    void start_communicating();
    void close_connection();
    void login();
    static void send_handler(Client* client);
    static void recv_handler(Client* client);
    void multi_print(string message, bool you=true);
    ~Client();

    thread* send_thread;
    thread* recv_thread;
    int MAX_LEN;
    bool logged_in;
    bool exited;
    char* name;
    char* username;
    char* password;
    char* passwordRepeat;
    mutex print_mtx;
    int client_socket;
};

#endif //CLIENT_CLIENT_H
