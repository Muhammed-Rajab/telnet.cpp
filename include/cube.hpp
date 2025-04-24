#ifndef CUBE_HPP
#define CUBE_HPP

#include <iostream>
#include <sstream>
#include <cmath>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>

namespace Cube
{

    const int width = 60;
    const int height = 33;

    char bg_ascii_char = ' ';

    char buffer[width * height] = {};

    float A = 0;
    float B = 0;
    float C = 0;

    float cube_side = 7;

    //--------------------------------------->
    // CUBE DATA
    //--------------------------------------->
    struct Vec2
    {
        float x;
        float y;
    };

    struct Vertex
    {
        float x, y, z;
    };

    Vertex cubeVertices[] = {
        {-1.0f, -1.0f, -1.0f}, // 0: Front-bottom-left
        {1.0f, -1.0f, -1.0f},  // 1: Front-bottom-right
        {1.0f, 1.0f, -1.0f},   // 2: Back-bottom-right
        {-1.0f, 1.0f, -1.0f},  // 3: Back-bottom-left
        {-1.0f, -1.0f, 1.0f},  // 4: Front-top-left
        {1.0f, -1.0f, 1.0f},   // 5: Front-top-right
        {1.0f, 1.0f, 1.0f},    // 6: Back-top-right
        {-1.0f, 1.0f, 1.0f}    // 7: Back-top-left
    };

    int cubeEdges[][2] = {
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

    void play(int client_socket)
    {
        // UPDATE VERTEX BASED ON CUBE SIZE
        for (Vertex &v : cubeVertices)
        {
            v.x = v.x * cube_side / 2;
            v.y = v.y * cube_side / 2;
            v.z = v.z * cube_side / 2;
        }

        while (true)
        {
            // RESET BUFFER
            memset(buffer, bg_ascii_char, width * height);

            // DRAWING LOGIC GOES HERE ------------------------->
            Vec2 projected[8];

            for (int i = 0; i < 8; i += 1)
            {
                Vertex &v = cubeVertices[i];

                float x = calculateX(v.x, v.y, v.z);
                float y = calculateY(v.x, v.y, v.z);
                float z = calculateZ(v.x, v.y, v.z) + 25;

                float ooz = 1 / z;

                int K1 = 60;

                int xp = (int)(width / 2 + ooz * x * 2 * K1);
                int yp = (int)(height / 2 + ooz * y * K1);

                projected[i] = {(float)xp, (float)yp};
            }

            for (int i = 0; i < 12; i += 1)
            {
                int *edge = cubeEdges[i];

                Vec2 A = projected[edge[0]];
                Vec2 B = projected[edge[1]];

                int x0 = A.x;
                int y0 = A.y;

                int x1 = B.x;
                int y1 = B.y;

                char ch = '#';

                // BRESENHAM'S LINE ALGORITHM
                int dx = std::abs(x1 - x0), dy = -std::abs(y1 - y0);
                int sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1;
                int err = dx + dy;

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

            // RENDER
            std::string canvas = render();
            send(client_socket, canvas.c_str(), canvas.length(), 0);

            // UPDATE
            A += 0.05;
            B += 0.05;
            C += 0.01;

            // DELAY
            usleep(8000 * 5);
        }
    }
}

#endif