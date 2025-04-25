#include <iostream>
#include <unistd.h>
#include <sys/socket.h>

#include "include/utils.hpp"
#include "include/server.hpp"

#include "include/cube.hpp"
#include "include/kitty_says.hpp"

void handler(int client_socket)
{
    bool running = true;
    do
    {
        clear_screen(client_socket);

        std::string menu = R"(
[:: telnet.cpp interface ::]

  1 > kitty.says()
  2 > cube.render()
  3 > system.exit()

~ run [1-3] >> )";

        std::string choice = prompt(client_socket, menu, 2);

        clear_screen(client_socket);

        if (choice == "1")
        {
            if (!Kittysays::play(client_socket))
            {
                break;
            }
            flush_socket(client_socket);
        }
        else if (choice == "2")
        {
            Cube c;
            if (!c.play(client_socket))
            {
                break;
            }
            flush_socket(client_socket);
        }
        else
        {
            clear_screen(client_socket);
            std::string message = "CIAO!\n\n";
            send(client_socket, message.c_str(), message.length(), 0);
            break;
        }
    } while (true);

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

    Server s(1337);

    s.listen(handler);

    return EXIT_SUCCESS;
}