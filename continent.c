void draw_continent(float* values, int img_size, struct Pos pos, int c_width, int c_height)
{
    double max_dist = dist_between((struct Pos){pos.x - c_width, pos.y - c_height}, pos);

    unsigned char* pixels = malloc(3 * img_size * img_size);
    unsigned int index = 0;

    for (int y = 0; y < img_size; y++)
    {
        for (int x = 0; x < img_size; x++)
        {
            int dist_x = abs(pos.x - x);
            int dist_y = abs(pos.y - y);

            unsigned char color;

            if (dist_x <= c_width && dist_y <= c_height)
            {
                double dist = dist_between((struct Pos){x, y}, pos);
                dist = -dist + max_dist;
                dist /= max_dist;

                values[x * img_size + y] = (float)dist;

                color = (unsigned char)(dist * 255);
            }
            else
            {
                color = 0;
            }

            pixels[index] = color;
            pixels[index + 1] = color;
            pixels[index + 2] = color;

            index += 3;
        }
    }

    stbi_write_png("continent.png", img_size, img_size, 3, pixels, 3 * img_size);
    free(pixels);
}

/* draw an image displaying the area that the continents take up and return the values through the parameter */
void draw_continents(int64_t seed, float* values)
{
    srand(seed << 3);

    int num_continents = (rand() % (C_MAX_NUM - C_MIN_NUM)) + C_MIN_NUM;
    struct Continent* continents = malloc(sizeof(struct Continent) * num_continents);

    for (int i = 0; i < num_continents; i++)
    {
        int width;
        int height;
        int diff = C_MAX_DIFF + 1;

        while (diff > C_MAX_DIFF)
        {
            width = (rand() % (C_MAX_RAD - C_MIN_RAD)) + C_MIN_RAD;
            height = (rand() % (C_MAX_RAD - C_MIN_RAD)) + C_MIN_RAD;

            diff = abs(width - height);
        }

        // position has to be offset from the border by the width/height radius + a constant amount
        int buffer_dist = 50;

        int min_x, max_x, min_y, max_y;

        min_x = width + buffer_dist;
        max_x = IMG_SIZE - min_x;
        min_y = height + buffer_dist;
        max_y = IMG_SIZE - min_y;

        struct Pos pos = {rand() % (max_x - min_x) + min_x, rand() % (max_y - min_y) + min_y};

        double max_dist = dist_between((struct Pos){pos.x - width, pos.y - height}, pos);

        continents[i] = (struct Continent){pos, width, height, max_dist};
    }

    unsigned char* pixels = malloc(3 * IMG_SIZE * IMG_SIZE);
    unsigned int index = 0;

    for (int y = 0; y < IMG_SIZE; y++)
    {
        for (int x = 0; x < IMG_SIZE; x++)
        {
            float highest_val = 0.0f;

            for (int i = 0; i < num_continents; i++)
            {
                struct Continent temp = continents[i];
                double temp_dist = dist_between((struct Pos){x, y}, temp.center);

                float val = -temp_dist + temp.max_dist;
                val /= temp.max_dist;

                if (val > highest_val)
                {
                    highest_val = val;
                }
            }

            values[x * IMG_SIZE + y] = highest_val;
            unsigned char color = (unsigned char)(highest_val * 255);

            pixels[index] = color;
            pixels[index + 1] = color;
            pixels[index + 2] = color;

            index += 3;
        }
    }

    stbi_write_png("continents.png", IMG_SIZE, IMG_SIZE, 3, pixels, 3 * IMG_SIZE);
    //free(pixels);
}