#ifndef KITTY_SAYS_HPP
#define KITTY_SAYS_HPP

#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <unistd.h>
#include <algorithm>
#include <sys/socket.h>

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
            throw "the fuck you doin? set count <= collection.size()";
        }

        std::vector<std::string> result;

        // Random engine and distribution
        std::random_device rd;
        std::mt19937 rng(rd());

        // Sample `n` random strings from the vector without repetition
        std::sample(collection.begin(), collection.end(), std::back_inserter(result), count, rng);

        return result;
    }

    std::vector<std::string> getStoicQuotes(size_t count)
    {
        if (count > stoic.size())
        {
            throw "we have only 8 stoic quotes. keep count <= 8";
        }

        return getQuotes(stoic, count);
    }

    std::vector<std::string> getNihilQuotes(size_t count)
    {
        if (count > nihil.size())
        {
            throw "we have only 8 nihilistic quotes. keep count <= 8";
        }

        return getQuotes(nihil, count);
    }

    std::vector<std::string> getExistentialQuotes(size_t count)
    {
        if (count > nihil.size())
        {
            throw "we have only 8 existential quotes. keep count <= 8";
        }

        return getQuotes(existential, count);
    }

    std::vector<std::string> getAbsurdQuotes(size_t count)
    {
        if (count > absurd.size())
        {
            throw "we have only 8 absurd quotes. keep count <= 8";
        }

        return getQuotes(absurd, count);
    }
};

void playKittySays(int client_socket)
{

    std::vector<std::vector<std::string>> kittyFrames = {
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

    int frame_count = kittyFrames.size();
    int frameIndex = 0;

    std::string clear_move = "\x1B[2J\x1B[H\x1B[3J";

    auto quotes = Quotes::getNihilQuotes(2);

    std::string endMenu = "\n[q]uit         [c]hange";

    char ch;
    while (true)
    {
        std::vector<std::string> frame = kittyFrames.at(frameIndex);

        std::string composed_frame = "\n" + frame[0] + "\n" + frame[1] + quotes[0] + "\n" + frame[2] + quotes[1] + "\n";

        std::string composed = (clear_move + composed_frame + endMenu);

        send(client_socket, composed.c_str(), composed.length(), 0);
        usleep(300000);

        frameIndex = (frameIndex + 1) % frame_count;

        ssize_t bytes = recv(client_socket, &ch, 1, MSG_DONTWAIT);
        if (bytes > 0 && (ch == 'q' || ch == 'Q'))
            break;
        if (bytes > 0 && (ch == 'c' || ch == 'C'))
            quotes = Quotes::getStoicQuotes(2);
    }

    close(client_socket);
}

#endif