#ifndef SERVER_HPP
#define SERVER_HPP

#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <functional>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <thread>
#include <unistd.h>

#include "utils.hpp"

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

    if (bind(this->server_fd, (struct sockaddr *)&(this->address),
             this->addrlen)) {
      perror("bind failed");
      exit(EXIT_FAILURE);
    }
  }

  void listen(std::function<void(int)> handler) {
    ::listen(this->server_fd, 3);

    std::cout << "Server started successfully!" << std::endl;
    std::cout << "Run `telnet localhost " << port << '`' << std::endl;

    while (true) {
      int client_socket =
          accept(this->server_fd, (struct sockaddr *)&(this->address),
                 (socklen_t *)&(this->addrlen));

      if (client_socket < 0) {
        perror("accept");
        continue;
      }

      // Get the client's IP and port
      char client_ip[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, &(this->address.sin_addr), client_ip, INET_ADDRSTRLEN);
      uint16_t client_port = ntohs(this->address.sin_port);

      // Log the connection details with timestamp
      std::string timestamp = get_current_timestamp();
      std::cout << "[" << timestamp << "] NEW CONNECTION from " << client_ip
                << ":" << client_port << std::endl;

      // Set a timeout for receiving data (e.g., 30 seconds)
      struct timeval timeout;
      timeout.tv_sec = 30;
      timeout.tv_usec = 0;
      if (setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                     sizeof(timeout)) < 0) {
        perror("setsockopt(SO_RCVTIMEO) failed");
      }

      std::thread([handler, client_socket]() {
        handler(client_socket);
      }).detach();
    }
  }

  ~Server() { close(this->server_fd); }
};

#endif
