#include "client.h"

Client::Client()
{
    MAX_LEN = 150;
    logged_in = false;
    name = new char[MAX_LEN];
    username = new char[MAX_LEN];
    exited = false;
    password = new char[MAX_LEN];
    passwordRepeat = new char[MAX_LEN];
}

void Client::start_connecting() {
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket creation failed!");
        exit(-1);
    }
    struct sockaddr_in client;
    client.sin_family=AF_INET;
    client.sin_port=htons(10000);
    client.sin_addr.s_addr=INADDR_ANY;

    if ((connect(client_socket, (struct sockaddr *)&client, sizeof(struct sockaddr_in))) == -1)
    {
        perror("connecting failed!");
        exit(-1);
    }
}

void Client::start_communicating() {
    login();
    send_thread = new thread(send_handler, this);
    recv_thread = new thread(recv_handler, this);
    if (send_thread->joinable())
        send_thread->join();
    if (recv_thread->joinable())
        recv_thread->join();
}

void Client::login() {
    while (true) {
        cout << "Enter your username : ";
        cin.getline(username, MAX_LEN);
        if (string(username).size() == 0) {
            cout << "(Error : Problem Occurred) You did not enter anything!" << endl;
            continue;
        }
        cout<< "Enter your password : ";
        cin.getline(password, MAX_LEN);
        if (string(password).size() == 0) {
            cout << "(Error : Problem Occurred) You did not enter anything!" << endl;
            continue;
        }
        cout<< "Re-enter your password : ";
        cin.getline(passwordRepeat, MAX_LEN);
        if (string(password) != string(passwordRepeat)) {
            cout << "(Error : Problem Occurred) Passwords do not match, try logging in again." << endl;
            continue;
        }
        send(client_socket, username, MAX_LEN, 0);
        send(client_socket, password, MAX_LEN, 0);
        bool dd = false;
        while (string(name).size() == 0) {
            if (dd) cout << "(Error : Problem Occurred) You did not enter anything!" << endl;
            dd = true;
            cout << "Enter your visible profile name for this session : ";
            cin.getline(name, MAX_LEN);
        }
        dd = false;
        send(client_socket, name, MAX_LEN, 0);
        multi_print("Your login request has been sent!", false);
        char answer[MAX_LEN];
        int bytes_received = recv(client_socket, answer, sizeof(answer), 0);
        if(bytes_received <= 0)
            continue;
        multi_print(answer, false);
        if (string(answer) == "Server | Welcome " + string(name) + " (@" + string(username) + ")")
            break;
    }
}

void Client::multi_print(string message, bool you) {
    lock_guard<mutex> guard(print_mtx);
    if (message.length())
        cout << "\33[2K \r" << message << endl;
    if (you)
        cout << "\33[2K \r" << "You : ";
}

void Client::send_handler(Client* client) {
    while(true) {
        client->multi_print("");
        char str[client->MAX_LEN];
        cin.getline(str,client->MAX_LEN);
        if (string(str).size() == 0) continue;
        if (string(str) == "#exit") {
            client->exited = true;
            client->recv_thread->detach();
            close(client->client_socket);
            return;
        }
        send(client->client_socket, str, sizeof(str), 0);
    }
}

void Client::recv_handler(Client* client) {
    while (!client->exited) {
        char message[client->MAX_LEN];
        int bytes_received = recv(client->client_socket, message, sizeof(message), 0);
        if(bytes_received <= 0)
            continue;
        client->multi_print(message);
        fflush(stdout);
    }
}

void Client::close_connection() {
    if (send_thread) {
        if (send_thread->joinable()) {
            send_thread->detach();
            delete send_thread;
        }
        send_thread = 0;
    }
    if (recv_thread) {
        if (recv_thread->joinable()) {
            recv_thread->detach();
            delete recv_thread;
        }
        recv_thread = 0;
    }
    close(client_socket);
    multi_print("==== The socket turned off ====", false);
}

Client::~Client() {
    close_connection();
    delete [] name;
    delete [] password;
}


