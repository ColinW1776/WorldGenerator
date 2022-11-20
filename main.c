#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "simplex.c"

/* Image size needs to be global */
const int IMG_SIZE = 2048;

struct Pos
{
    int x;
    int y;
};

struct Point
{
    struct Pos pos;
    int radius;
    double max_dist;
};

struct Continent
{
    struct Point* cluster;
    int num_points;
};

double dist_between(struct Pos first, struct Pos second)
{
    return sqrt(((second.x - first.x) * (second.x - first.x)) + ((second.y - first.y) * (second.y - first.y)));
}

/* Voronoi polygon settings */
const int CELL_SIZE = 16;
const int NUM_CELLS = IMG_SIZE / CELL_SIZE;

#include "continent.c"
#include "voronoi.c"

int64_t get_seed()
{
    int64_t this;
    int first, second;

    // first 32 bits;
    first = rand();
    first <<= 16;
    second = rand();
    first += second;

    // put the first 32 bits into the 64 bit int, then shift it 32 bits
    this = (int64_t)first;
    this <<= 32;

    // second 32 bits
    first = rand();
    first <<= 16;
    second = rand();
    first += second;

    this += first;
    return this;
}

int main()
{
    struct timespec start, finish;
    clock_gettime(CLOCK_MONOTONIC, &start);
    unsigned long long begin = start.tv_sec * 1000000000 + start.tv_nsec;

    /* Start map drawing */

    srand(time(0));
    int64_t seed = get_seed();

    printf("seed: %lld\n", seed);

    /* Simplex settings creation */
    int num_octaves = 3;
    float thingy = 96.0f;
    unsigned char* perms = get_permutations(seed);

    /* Continent generation */
    struct Continent* continent = malloc(sizeof(struct Continent));
    generate_continent(seed, continent);

    /* Voronoi point generation */
    srand(seed >> 1); // reseed the random number generator for point generation

    struct Pos* points = malloc(sizeof(struct Pos) * NUM_CELLS * NUM_CELLS);
    float* values = malloc(sizeof(float) * NUM_CELLS * NUM_CELLS);

    for (int x = 0; x < NUM_CELLS; x++)
    {
        for (int y = 0; y < NUM_CELLS; y++)
        {
            struct Pos pos = {rand() % CELL_SIZE, rand() % CELL_SIZE};
            points[x * NUM_CELLS + y] = pos;

            float cont_val = continent_value_at(continent, (x * CELL_SIZE + pos.x), (y * CELL_SIZE + pos.y));
            float simplex_val = (octave((float)(x * CELL_SIZE + pos.x) / thingy, (float)(y * CELL_SIZE + pos.y) / thingy, num_octaves, perms) + 1) / 2;

            values[x * NUM_CELLS + y] = cont_val - simplex_val;
        }
    }

    draw_polygons(values, points);

    /* End of map drawing */

    clock_gettime(CLOCK_MONOTONIC, &finish);
    unsigned long long end = finish.tv_sec * 1000000000 + finish.tv_nsec;

    unsigned long long elapsed = end - begin;
    printf("time elapsed (nanoseconds): %llu\n", elapsed);
    elapsed /= 1000000;
    printf("time elapsed (milliseconds): %llu\n", elapsed);

    return 0;
}
