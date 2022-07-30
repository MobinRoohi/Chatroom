#include "user_server.h"

User_server::User_server(int _id, int _client_socket):
    id(_id), client_socket(_client_socket), name("Anonymous"), username("Anonymous") {}

User_server::~User_server()
{
    if (client_thread) {
        if (client_thread->joinable()) {
            client_thread->detach();
            delete client_thread;
        }
        client_thread = 0;
    }
    if (client_socket)
        close(client_socket);
}