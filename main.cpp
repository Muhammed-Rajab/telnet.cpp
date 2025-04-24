#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <functional>
#include <sys/socket.h>
#include <netinet/in.h>

#include "include/utils.hpp"

#include "include/cube.hpp"
#include "include/kitty_says.hpp"

#define LOG std::cout
#define NL "\n"

struct Server
{
private:
    int server_fd;
    uint16_t port;

    int addrlen;
    sockaddr_in address;

public:
    Server(uint16_t port_) : port(port_)
    {
        this->server_fd = socket(AF_INET, SOCK_STREAM, 0);

        int opt = 1;
        this->addrlen = sizeof(this->address);

        setsockopt(this->server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

        this->address.sin_family = AF_INET;
        this->address.sin_addr.s_addr = INADDR_ANY;
        this->address.sin_port = htons(this->port);

        bind(this->server_fd, (struct sockaddr *)&(this->address), this->addrlen);
    }

    void listen(std::function<void(int)> handler)
    {
        ::listen(this->server_fd, 3);

        while (true)
        {
            int client_socket = accept(this->server_fd, (struct sockaddr *)&(this->address), (socklen_t *)&(this->addrlen));

            if (client_socket < 0)
            {
                perror("accept");
                continue;
            }

            handler(client_socket);
        }
    }

    ~Server()
    {
        close(this->server_fd);
    }
};

void handler(int client_socket)
{
    // CLEAR SCREEN
    clear_screen(client_socket);

    clear_screen(client_socket);

    // TODO: MAKE THIS SHIT LOOK APPETIZING
    std::string menu = R"(
=------------------------------------=
|            telnet.cpp              |
=------------------------------------=

    1. kitty says...
    2. cube.cpp
    3. bye!

=------------------------------------=

Choose an option [1-3]: )";

    std::string choice = prompt(client_socket, menu, 2);

    clear_screen(client_socket);

    if (choice == "1")
    {
        playKittySays(client_socket);
    }
    else if (choice == "2")
    {
        playCube(client_socket);
    }
    else
    {
        clear_screen(client_socket);
        std::string message = "CIAO CIAO!\n";
        send(client_socket, message.c_str(), message.length(), 0);
    }

    close(client_socket);
}

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

    Server s(1337);

    s.listen(handler);

    return EXIT_SUCCESS;
}