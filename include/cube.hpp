#ifndef CUBE_HPP
#define CUBE_HPP

#include <iostream>
#include <sstream>
#include <cmath>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>

#include "colors.hpp"

struct Vec2
{
    float x;
    float y;
};

struct Vertex
{
    float x, y, z;
};

struct Cube
{
private:
    const int width = 60;
    const int height = 30;

    const char bg_ascii_char = ' ';

    char *buffer;

    float A = 0;
    float B = 0;
    float C = 0;

    const int K1 = 60;
    const int Z_OFFSET = 25;

    const float cube_side = 7.5;

    std::vector<Vertex> vertices;
    std::vector<std::vector<int>> edges;

    bool is_playing = true;
    float speed = 1.0f;

    std::string render()
    {
        std::ostringstream oss;

        oss << "\x1b[H";
        for (int y = 0; y < height; y += 1)
        {
            for (int x = 0; x < width; x += 1)
            {
                oss << buffer[y * width + x];
            }
            oss << "\n";
        }

        return oss.str();
    }

    float calculateX(int i, int j, int k)
    {
        return j * sin(A) * sin(B) * cos(C) - k * cos(A) * sin(B) * cos(C) + j * cos(A) * sin(C) + k * sin(A) * sin(C) + i * cos(B) * cos(C);
    }

    float calculateY(int i, int j, int k)
    {
        return j * cos(A) * cos(C) + k * sin(A) * cos(C) - j * sin(A) * sin(B) * sin(C) + k * cos(A) * sin(B) * sin(C) - i * cos(B) * sin(C);
    }

    float calculateZ(int i, int j, int k)
    {
        return k * cos(A) * cos(B) - j * sin(A) * cos(B) + i * sin(B);
    }

public:
    Cube()
    {
        buffer = new char[width * height];

        vertices = {
            {-1.0f, -1.0f, -1.0f}, // 0: Front-bottom-left
            {1.0f, -1.0f, -1.0f},  // 1: Front-bottom-right
            {1.0f, 1.0f, -1.0f},   // 2: Back-bottom-right
            {-1.0f, 1.0f, -1.0f},  // 3: Back-bottom-left
            {-1.0f, -1.0f, 1.0f},  // 4: Front-top-left
            {1.0f, -1.0f, 1.0f},   // 5: Front-top-right
            {1.0f, 1.0f, 1.0f},    // 6: Back-top-right
            {-1.0f, 1.0f, 1.0f}    // 7: Back-top-left
        };

        edges = {
            {0, 1}, {1, 2}, {2, 3}, {3, 0}, // back face
            {4, 5},
            {5, 6},
            {6, 7},
            {7, 4}, // front face
            {0, 4},
            {1, 5},
            {2, 6},
            {3, 7} // connecting edges
        };

        // UPDATE VERTEX BASED ON CUBE SIZE
        for (Vertex &v : vertices)
        {
            v.x = v.x * cube_side / 2;
            v.y = v.y * cube_side / 2;
            v.z = v.z * cube_side / 2;
        }
    }

    ~Cube()
    {
        delete[] buffer;
    }

    bool play(int client_socket)
    {

        while (true)
        {
            // RESET BUFFER
            memset(buffer, bg_ascii_char, width * height);

            // DRAWING LOGIC GOES HERE ------------------------->

            // TODO: SET SIZE BASED ON VERTICES
            std::vector<Vec2> projected_vertices;

            for (int i = 0; i < vertices.size(); i += 1)
            {
                Vertex &v = vertices[i];

                float x = calculateX(v.x, v.y, v.z);
                float y = calculateY(v.x, v.y, v.z);
                float z = calculateZ(v.x, v.y, v.z) + Z_OFFSET;

                float ooz = 1 / z;

                int xp = (int)(width / 2 + ooz * x * 2 * K1);
                int yp = (int)(height / 2 + ooz * y * K1);

                projected_vertices.push_back({(float)xp, (float)yp});
            }

            for (int i = 0; i < edges.size(); i += 1)
            {
                const std::vector<int> &edge = edges[i];

                Vec2 A = projected_vertices[edge[0]];
                Vec2 B = projected_vertices[edge[1]];

                int x0 = A.x;
                int y0 = A.y;

                int x1 = B.x;
                int y1 = B.y;

                // BRESENHAM'S LINE ALGORITHM
                int dx = std::abs(x1 - x0), dy = -std::abs(y1 - y0);
                int sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1;
                int err = dx + dy;

                char ch = '#';

                while (true)
                {
                    if (x0 >= 0 && x0 < width && y0 >= 0 && y0 < height)
                        buffer[y0 * width + x0] = ch;

                    if (x0 == x1 && y0 == y1)
                        break;
                    int e2 = 2 * err;
                    if (e2 >= dy)
                    {
                        err += dy;
                        x0 += sx;
                    }
                    if (e2 <= dx)
                    {
                        err += dx;
                        y0 += sy;
                    }
                }
            }

            // WATERMARK
            std::string watermark = "CUBE";
            int index = (height / 2) * width + width / 2 - watermark.size() / 2;
            for (size_t i = 0; i < watermark.size(); ++i)
                buffer[index + i] = watermark[i];

            // RENDER
            std::string canvas = render();
            std::string endMenu = Colors::Bright_Black + "\n[q]uit         [m]enu        [<space>] play/pause          [[/]] speed ↑/↓\n" + Colors::Reset;
            canvas = canvas + endMenu;

            send(client_socket, canvas.c_str(), canvas.length(), 0);

            // UPDATE
            if (is_playing)
            {
                A += 0.05 * speed;
                B += 0.05 * speed;
                C += 0.01 * speed;
            }

            // INPUT HANDLING
            char input_buffer[1024];
            int bytes_received = recv(client_socket, input_buffer, sizeof(input_buffer), MSG_DONTWAIT);

            if (bytes_received > 0)
            {
                // check for ctrl+c
                if ((unsigned char)input_buffer[0] == 0xFF && (unsigned char)input_buffer[1] == 0xF4)
                {
                    std::cout << "Ctrl+Ced from cube" << std::endl;
                    close(client_socket);
                    break;
                }

                // check for 'q' or 'm'
                for (int i = 0; i < bytes_received; ++i)
                {
                    char c = input_buffer[i];
                    if (c == 'q')
                    {
                        flush_socket(client_socket);
                        return false;
                    }
                    else if (c == 'm')
                    {
                        flush_socket(client_socket);
                        return true;
                    }
                    else if (c == ' ')
                    {
                        is_playing = !is_playing;
                        flush_socket(client_socket);
                    }
                    else if (c == '[')
                    {
                        if (speed - 0.25 < 0)
                        {
                            speed = 0;
                        }
                        else
                        {
                            speed = speed - 0.25;
                        }
                        flush_socket(client_socket);
                    }
                    else if (c == ']')
                    {
                        if (speed + 0.25 > 3)
                        {
                            speed = 3;
                        }
                        else
                        {
                            speed = speed + 0.25;
                        }
                        flush_socket(client_socket);
                    }
                }

                flush_socket(client_socket);
            }
            else if (bytes_received == 0)
            {
                // Client disconnected
                close(client_socket);
                std::cout << "DISCONNECTED!" << std::endl;
                break;
            }
            else if (bytes_received < 0 && errno != EWOULDBLOCK && errno != EAGAIN)
            {
                // Real error occurred
                perror("recv failed");
                close(client_socket);
                break;
            }

            // DELAY
            usleep(8000 * 5);
        }
        return false;
    }
};

#endif