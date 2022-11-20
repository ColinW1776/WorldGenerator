void draw_polygons(float* values, struct Pos* points)
{
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
}