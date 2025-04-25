#ifndef KITTY_SAYS_HPP
#define KITTY_SAYS_HPP

#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <unistd.h>
#include <algorithm>
#include <sys/socket.h>

#include "colors.hpp"

namespace Kittysays
{
    const std::vector<std::vector<std::string>> kitty_frames = {
        {
            "  /\\_/\\ ",
            " ( o.o ) ",
            "  > ^ <  ",
        },
        {
            "  /\\_/\\ ",
            " ( -.- ) ",
            "  > ^ <  ",
        },
        {
            "  /\\_/\\ ",
            " ( -.- ) ",
            "  > ^ <  ",
        },
        {
            "  /\\_/\\ ",
            " ( o.o ) ",
            "  > ^ <  ",
        },
    };

    namespace Quotes
    {
        std::vector<std::string> absurd = {
            "we live in the question. answers are illusions.",
            "meaning is a trick we play on ourselves.",
            "existence is the joke. we just don't get it.",
            "we seek purpose, but find only the void.",
            "every path leads to nowhere. embrace it.",
            "time is a circle. we are stuck in its loop.",
            "we run, yet the horizon never moves."};

        std::vector<std::string> nihil = {
            "life is a moment. then it's nothing.",
            "the universe doesn’t hear our cries.",
            "all things fade into the same darkness.",
            "no reason to be. no reason not to.",
            "we search for meaning in an empty room.",
            "death laughs at our efforts.",
            "there is no end. there is no beginning.",
            "existence is an accident of the cosmos."};

        std::vector<std::string> existential = {
            "you are nothing but a moment of choice.",
            "freedom is the burden of being.",
            "without meaning, we must create it.",
            "to exist is to choose — endlessly.",
            "the world does not owe you purpose.",
            "we are condemned to be free.",
            "life is a question. answers are your own.",
            "we are thrown into the world, naked and alone."};

        std::vector<std::string> stoic = {
            "what happens, happens. learn to endure.",
            "the only thing we control is our response.",
            "fate is indifferent. accept it.",
            "we are waves on an endless sea.",
            "the world is indifferent, but you can be at peace.",
            "all is fleeting — remember that and endure.",
            "calm the storm within, no matter the chaos outside.",
            "suffering is inevitable. growth is optional."};

        std::vector<std::string> getQuotes(const std::vector<std::string> &collection, size_t count)
        {
            if (count > collection.size())
            {
                throw std::runtime_error("the fuck you doin? set count <= collection.size()");
            }

            std::vector<std::string> result;

            // Random engine and distribution
            std::random_device rd;
            std::mt19937 rng(rd());

            // Sample `n` random strings from the vector without repetition
            std::sample(collection.begin(), collection.end(), std::back_inserter(result), count, rng);

            return result;
        }

        std::vector<std::string> getRandomQuotes(size_t count)
        {
            // XXX:
            if (count > absurd.size())
            {
                throw std::runtime_error("we only have 8 quotes per category. keep count <= 8");
            }

            std::vector<std::string> *categories[4] = {
                &nihil, &absurd, &existential, &stoic};

            std::random_device rd;  // true random seed
            std::mt19937 gen(rd()); // Mersenne Twister RNG
            std::uniform_int_distribution<> dist(0, 3);

            return getQuotes(*(categories[dist(gen)]), count);
        }
    };

    bool play(int client_socket)
    {

        int frame_count = kitty_frames.size();
        int frame_index = 0;

        std::string clear_move = "\x1B[2J\x1B[H\x1B[3J";

        auto quotes = Quotes::getRandomQuotes(2);

        std::string kitty_says = Colors::Bright_Black + Colors::Italic + "kitty says...\n" + Colors::Reset;
        std::string endMenu = Colors::Bright_Black + "\n[q]uit         [c]hange        [m]enu" + Colors::Reset;

        char ch = '\0';

        while (true)
        {
            std::vector<std::string> frame = kitty_frames.at(frame_index);

            std::string composed_frame = Colors::Bold + Colors::Italic + "\n" + frame[0] + "\n" + frame[1] + Colors::Red + quotes[0] + "\n" + Colors::Reset + frame[2] + Colors::Bright_White + Colors::Underline + quotes[1] + "\n" + Colors::Reset;

            std::string composed = clear_move + kitty_says + composed_frame + endMenu;

            send(client_socket, composed.c_str(), composed.length(), 0);
            usleep(300000);

            frame_index = (frame_index + 1) % frame_count;

            ssize_t bytes = recv(client_socket, &ch, 1, MSG_DONTWAIT);
            if (bytes > 0 && (ch == 'q' || ch == 'Q'))
            {
                flush_socket(client_socket);
                return false;
            }
            if (bytes > 0 && (ch == 'c' || ch == 'C'))
            {
                flush_socket(client_socket);
                quotes = Quotes::getRandomQuotes(2);
            }
            if (bytes > 0 && (ch == 'm' || ch == 'M'))
            {
                flush_socket(client_socket);
                return true;
            }
        }

        return false;
    }
}

#endif