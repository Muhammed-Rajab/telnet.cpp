#ifndef SERVER_HPP
#define SERVER_HPP

#include <cstdio>
#include <fcntl.h>
#include <functional>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

struct Server {
private:
  int server_fd;
  uint16_t port;

  int addrlen;
  sockaddr_in address;

public:
  Server(uint16_t port_) : port(port_) {
    this->server_fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    this->addrlen = sizeof(this->address);

    setsockopt(this->server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
               sizeof(opt));

    this->address.sin_family = AF_INET;
    this->address.sin_addr.s_addr = INADDR_ANY;
    this->address.sin_port = htons(this->port);

    bind(this->server_fd, (struct sockaddr *)&(this->address), this->addrlen);
  }

  void listen(std::function<void(int)> handler) {
    ::listen(this->server_fd, 3);

    while (true) {
      int client_socket =
          accept(this->server_fd, (struct sockaddr *)&(this->address),
                 (socklen_t *)&(this->addrlen));

      if (client_socket < 0) {
        perror("accept");
        continue;
      }

      handler(client_socket);
    }
  }

  ~Server() { close(this->server_fd); }
};

#endif
