#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define LOG std::cout
#define NL "\n"

int main()
{
    /*
    =------------=
    | telnet.cpp |
    =------------=

    wanna show cool graphics via TCP? here you got it!

    GOALS: Be able to stream text or animation via TCP socket and telnet.
    */

    LOG << "HELLO WORLD!" << NL;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(1337);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));

    listen(server_fd, 3);

    int client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);

    const char *message = "Hello, telnet boi!\n";
    send(client_socket, message, strlen(message), 0);

    char buffer[1024] = {0};
    int byte_read = read(client_socket, buffer, 1024);

    std::cout << "Received: " << buffer << std::endl;

    close(client_socket);
    close(server_fd);
    return EXIT_SUCCESS;
}