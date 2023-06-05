#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <iostream>

using namespace std;

int server_socket;
string socket_name;

volatile sig_atomic_t stop = 0;

void cleanup_socket() {
    close(server_socket);
    unlink(socket_name.c_str());
}

void handle_signal(int signal) {
    cout << "SIG! " << signal << endl;
    cleanup_socket();
    exit(0);
}

const int BUFFER_SIZE = 1024;

int main(int argc, char **argv) {
    int pid = getpid();
    cout << "Server PID: " << pid << endl;


    socket_name = "/tmp/my_socket";

    cout << "Socket: " << socket_name << endl;

    unlink(socket_name.c_str());

    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_socket == -1) {
        cerr << "Ошибка: не удалось создать Unix-сокет" << endl;
        exit(1);
    }


    sockaddr_un sock_addr;
    memset(&sock_addr, 0, sizeof(sockaddr_un));
    sock_addr.sun_family = AF_UNIX;
    strncpy(sock_addr.sun_path, socket_name.c_str(), sizeof(sock_addr.sun_path) - 1);


    if (bind(server_socket, (sockaddr * ) & sock_addr, sizeof(sock_addr)) == -1) {
        cerr << "Ошибка: не удалось привязать Unix-сокет к адресу" << endl;
        cleanup_socket();
        exit(1);
    }


    if (listen(server_socket, 10) == -1) {
        cerr << "Ошибка: не удалось начать прослушивание Unix-сокета" << endl;
        cleanup_socket();
        exit(1);
    }


    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    cout << "Сервер прослушивает секет. Ctrl+C - выход." << endl;


    while (!stop) {
        int client_socket = accept(server_socket, NULL, NULL);
        cout << "Клиент подключился" << endl;
        if (client_socket == -1) {
            if (stop) {
                cleanup_socket();
                break;
            }
            cerr << "Ошибка: не удалось принять входящее соединение" << endl;
            continue;
        }


        char buffer[BUFFER_SIZE];
        int client_pid;
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received == -1) {
            cerr << "Ошибка: не удалось прочитать данные от клиента" << endl;
            close(client_socket);
            continue;
        }
        buffer[bytes_received] = '\0';


        cout << "Получено от клиента: " << buffer << endl;


        char pid_str[16];
        snprintf(pid_str, sizeof(pid_str), "%d", pid);
        if (write(client_socket, pid_str, strlen(pid_str)) < 0) {
            std::cerr << "Ошибка: не удалось отправить данные клиенту" << std::endl;
            exit(1);
        }


        close(client_socket);
    }

    cleanup_socket();

    cout << "Приложение завершено" << endl;
    return 0;
}