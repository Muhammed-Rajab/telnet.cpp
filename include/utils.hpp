#ifndef UTILS_HPP
#define UTILS_HPP

#include <arpa/inet.h>
#include <chrono>
#include <ctime>
#include <fcntl.h>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <unistd.h>

void flush_socket(int sockfd) {
  // Set socket to non-blocking
  int flags = fcntl(sockfd, F_GETFL, 0);
  fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

  char temp[1024];
  while (recv(sockfd, temp, sizeof(temp), 0) > 0) {
    // just drain
  }

  // Reset socket to original mode
  fcntl(sockfd, F_SETFL, flags);
}

void clear_screen(int client_socket) {
  std::string clear_move = "\x1B[2J\x1B[H\x1B[3J";
  int bytes_send =
      send(client_socket, clear_move.c_str(), clear_move.length(), 0);

  if (bytes_send <= 0) {
    perror("clear_screen");
  }
}

std::string prompt(int client_socket, std::string message, size_t buffer_size) {
  send(client_socket, message.c_str(), message.length(), 0);

  std::string input;
  char ch;
  ssize_t bytes;

  while ((bytes = read(client_socket, &ch, 1)) > 0) {
    if (ch == '\n')
      break;
    if (ch != '\r')
      input += ch;
  }

  return input;
}

std::string get_current_timestamp() {
  auto now = std::chrono::system_clock::now();
  std::time_t now_time = std::chrono::system_clock::to_time_t(now);
  std::stringstream ss;
  ss << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S");
  return ss.str();
}

// Global mutex to protect std::cout
std::mutex output_mutex;

// Thread-safe function to print messages
void cout_thread_safe(const std::string &message) {
  std::lock_guard<std::mutex> guard(output_mutex); // Lock the mutex
  std::cout << message << std::endl;               // Safe output to console
}

#endif
