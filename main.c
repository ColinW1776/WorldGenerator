#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "simplex.c"

const int IMG_SIZE = 2048;

/* Continent settings */

/*const int C_MIN_RAD = 100;
const int C_MAX_RAD = 200;
const int C_MIN_POS = 210;
const int C_MAX_POS = 301;
const int C_MAX_DIFF = 50;*/

/*const int C_MIN_RAD = 150;
const int C_MAX_RAD = 225;
const int C_MIN_POS = 231;
const int C_MAX_POS = 281;
const int C_MAX_DIFF = 35;*/

const int C_MIN_NUM = 4;
const int C_MAX_NUM = 6;

const int C_MIN_RAD = 250;
const int C_MAX_RAD = 600;
const int C_MAX_DIFF = 100;

const int CELL_SIZE = 16;
const int NUM_CELLS = IMG_SIZE / CELL_SIZE;

struct Pos
{
    int x;
    int y;
};

struct Continent
{
    struct Pos center;
    int width_radius;
    int height_radius;
    double max_dist;
};

double dist_between(struct Pos first, struct Pos second)
{
    return sqrt(((second.x - first.x) * (second.x - first.x)) + ((second.y - first.y) * (second.y - first.y)));
}

#include "continent.c"

int main()
{
    srand(time(0));
    int64_t seed = rand();

    /* Simplex image generation*/
    float* simplex_values = malloc(sizeof(float) * IMG_SIZE * IMG_SIZE);

    // either gotta increase octaves or increase the thingy in simplex.c when increasing image/continent size
    simplex(simplex_values, 3, IMG_SIZE, seed);
    simplex_image(simplex_values, IMG_SIZE);

    /* Generation of continents */
    float* continent_values = malloc(sizeof(float) * IMG_SIZE * IMG_SIZE);
    draw_continents(seed, continent_values);

    /* Continent generation */
    /*struct Pos continent;
    continent.x = (rand() % (C_MAX_POS - C_MIN_POS)) + C_MIN_POS;
    continent.y = (rand() % (C_MAX_POS - C_MIN_POS)) + C_MIN_POS;

    int width = 0;
    int height = 0;
    int diff = C_MAX_DIFF + 1;

    while (diff > C_MAX_DIFF)
    {
        width = (rand() % (C_MAX_RAD - C_MIN_RAD)) + C_MIN_RAD;
        height = (rand() % (C_MAX_RAD - C_MIN_RAD)) + C_MIN_RAD;

        diff = abs(width - height);
    }

    float* continent_values = malloc(sizeof(float) * IMG_SIZE * IMG_SIZE);
    draw_continent(continent_values, IMG_SIZE, continent, width, height);*/

    /* Overlay of Continent and Simplex */
    for (int x = 0; x < IMG_SIZE; x++)
    {
        for (int y = 0; y < IMG_SIZE; y++)
        {
            continent_values[x * IMG_SIZE + y] -= simplex_values[x * IMG_SIZE + y];
        }
    }


    // Voronoi diagram
    struct Pos* points = malloc(sizeof(struct Pos) * NUM_CELLS * NUM_CELLS);
    float* values = malloc(sizeof(float) * NUM_CELLS * NUM_CELLS);

    for (int x = 0; x < NUM_CELLS; x++)
    {
        for (int y = 0; y < NUM_CELLS; y++)
        {
            struct Pos pos = {rand() % CELL_SIZE, rand() % CELL_SIZE};

            points[x * NUM_CELLS + y] = pos;
            values[x * NUM_CELLS + y] = continent_values[(x * CELL_SIZE + pos.x) * IMG_SIZE + (y * CELL_SIZE + pos.y)];
        }
    }

    unsigned char* pixels = malloc(3 * IMG_SIZE * IMG_SIZE);
    unsigned int index = 0;

    for (int y = 0; y < IMG_SIZE; y++)
    {
        for (int x = 0; x < IMG_SIZE; x++)
        {
            int cell_x = x / CELL_SIZE;
            int cell_y = y / CELL_SIZE;

            float closest_val = 0.0f;
            double closest_to = 1000.0;

            for (int iy = -1; iy <= 1; iy++)
            {
                for (int ix = -1; ix <= 1; ix++)
                {
                    if (cell_x + ix >= 0 && cell_y + iy >= 0 && cell_x + ix < NUM_CELLS && cell_y + iy < NUM_CELLS)
                    {
                        struct Pos temp = points[(cell_x + ix) * NUM_CELLS + (cell_y + iy)];
                        temp.x += (cell_x + ix) * CELL_SIZE;
                        temp.y += (cell_y + iy) * CELL_SIZE;

                        double dist = dist_between((struct Pos){x, y}, temp);

                        if (dist < closest_to)
                        {
                            closest_to = dist;
                            closest_val = values[(cell_x + ix) * NUM_CELLS + (cell_y + iy)];
                        }
                    }
                }
            }

            unsigned char color;

            if (closest_val > 0)
                color = 255;
            else
                color = 0;

            pixels[index] = color;
            pixels[index + 1] = color;
            pixels[index + 2] = color;

            index += 3;
        }
    }

    stbi_write_png("polygons.png", IMG_SIZE, IMG_SIZE, 3, pixels, 3 * IMG_SIZE);
    free(pixels);

    return 0;
}
