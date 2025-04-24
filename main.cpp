#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <functional>
#include <sys/socket.h>
#include <netinet/in.h>

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

void kittyHandler(int client_socket)
{

    std::string kittyFrames[] = {

        R"(
 /\_/\ 
( o.o )
 > ^ <
)",

        R"(
 /\_/\  
( -.- ) 
 > ^ <  
)",

        R"(
 /\_/\ 
( o.o )
 > v < 
)",

        R"(
 /\_/\ 
( 0.0 )
 > ^ < 
)",

        R"(
 /\_/\ 
( owo )
 > ^ < 
)"};

    int frame_count = sizeof(kittyFrames) / sizeof(kittyFrames[0]);
    int frameIndex = 0;

    std::string clear_move = "\x1B[2J\x1B[H\x1B[3J";

    std::string message = "=- HELLO KITTY! -=";

    while (true)
    {
        std::string composed = (clear_move + kittyFrames[frameIndex] + message);
        send(client_socket, composed.c_str(), composed.length(), 0);
        frameIndex = (frameIndex + 1) % frame_count;
        usleep(300000);
    }

    // const char *message = "Hello, telnet boi!\n";
    // send(client_socket, message, strlen(message), 0);

    // char buffer[1024] = {0};
    // int byte_read = read(client_socket, buffer, 1024);

    // std::cout << "Received: " << buffer << std::endl;

    close(client_socket);
}

void handler(int client_socket)
{
    // ASK FOR COLOR
    std::string color_message = "ANSI Color supported terminal? [Yy|Nn]: ";
    send(client_socket, color_message.c_str(), color_message.length(), 0);

    char buffer[2] = {0};
    int byte_read = read(client_socket, buffer, 1);

    std::cout << "Received: " << buffer << std::endl;

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