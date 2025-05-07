#include <sys/socket.h>
#include <unistd.h>

#include "include/server.hpp"
#include "include/utils.hpp"

#include "include/cube.hpp"
#include "include/kitty_says.hpp"

void handler(int client_socket, std::string client_ip, uint16_t client_port) {
  do {
    clear_screen(client_socket);

    std::string menu = R"(
[:: telnet.cpp interface ::]

  1 > kitty.says()
  2 > cube.render()
  3 > system.exit()

~ run [1-3] >> )";

    std::string choice = prompt(client_socket, menu, 2);

    clear_screen(client_socket);

    if (choice == "1") {
      if (!Kittysays::play(client_socket)) {
        break;
      }
      flush_socket(client_socket);
    } else if (choice == "2") {
      Cube c;
      if (!c.play(client_socket)) {
        break;
      }
      flush_socket(client_socket);
    } else {
      clear_screen(client_socket);
      std::string message = "CIAO!\n\n";
      send(client_socket, message.c_str(), message.length(), 0);
      break;
    }
  } while (true);

  close(client_socket);

  // Log close message
  std::ostringstream oss;
  std::string timestamp = get_current_timestamp();
  oss << "[" << timestamp << "] DISCONNECTED from " << client_ip << ":"
      << client_port << "\n";
  cout_thread_safe(oss.str());
}

int main(int argc, char *argv[]) {

  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " <port>" << std::endl;
    return 1;
  }

  int port;
  try {
    port = std::stoi(argv[1]); // Convert the string to an integer
  } catch (const std::invalid_argument &e) {
    std::cerr << "INVALID PORT NUMBER" << std::endl;
    return 1;
  } catch (const std::out_of_range &e) {
    std::cerr << "PORT NUMBER OUT OF RANGE" << std::endl;
    return 1;
  }

  if (port < 1024 || port > 65535) {
    std::cerr << "PORT NUMBER MUST BE BETWEEN 1024 AND 65535." << std::endl;
    return 1;
  }

  // Server
  Server s(port);
  s.listen(handler);

  return EXIT_SUCCESS;
}
