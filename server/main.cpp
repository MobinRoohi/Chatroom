#include "server.h"
#include "signal.h"

Server* server = 0;

void exit_app(int sig_num);

int main() {
    server = new Server();
    signal(SIGTERM, exit_app);
//    server->get_users_from_file();
    server->start_listening();
    server->start_accepting();
}

void exit_app(int sig_num) {
    if (server)
        delete server;
    cout << "... Bye ..." << endl;
    exit(sig_num);
}
