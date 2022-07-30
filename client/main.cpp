#include "client.h"

Client* client = 0;

void exit_app(int sig_num);

int main() {
    client = new Client();
    client->start_connecting();
    client->start_communicating();
    exit_app(0);
}

void exit_app(int sig_num) {
    if (client)
        delete client;
    cout << "... Bye ..." << endl;
    exit(sig_num);
}