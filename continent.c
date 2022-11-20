void generate_continent(int64_t seed, struct Continent* continent)
{
    /* seed the random number generator */
    srand(seed << 2);

    /* constants used to define shape and size of continent */
    const int min_points = 6; // minimum number of points used to define the shape the continent
    const int max_points = 10; // maximum number of points used to define the shape of the continent
    const int min_radius = 300; // minimum radius of space covered by an individual point
    const int max_radius = 600; // maximum radius of space covered by an individual point
    const int buffer = 100; // minimum distance between edge of image and closest edge of continent
    const int compactness = 300; // maximum distance between two cluster points

    int num_points = rand() % (max_points - min_points) + min_points;
    continent->cluster = malloc(sizeof(struct Point) * num_points);
    continent->num_points = num_points;

    /* set the positions of each of the points */
    for (int i = 0; i < num_points; i++)
    {
        struct Point point;
        point.radius = rand() % (max_radius - min_radius) + min_radius;
        point.max_dist = dist_between((struct Pos){0, 0}, (struct Pos){point.radius, point.radius});

        int min_position = point.radius + buffer;
        int max_position = IMG_SIZE - min_position;

        if (i == 0)
        {
            point.pos = (struct Pos){rand() % (max_position - min_position) + min_position, rand() % (max_position - min_position) + min_position};
        }
        else
        {
            struct Point last = continent->cluster[i - 1];

            int min_x = min_position > (last.pos.x - compactness) ? min_position : (last.pos.x - compactness);
            int max_x = max_position < (last.pos.x + compactness) ? max_position : (last.pos.x + compactness);
            int min_y = min_position > (last.pos.y - compactness) ? min_position : (last.pos.y - compactness);
            int max_y = max_position < (last.pos.y + compactness) ? max_position : (last.pos.y + compactness);

            point.pos = (struct Pos){rand() % (max_x - min_x) + min_x, rand() % (max_y - min_y) + min_y};
        }

        continent->cluster[i] = point;
    }
}

float continent_value_at(struct Continent* continent, int x, int y)
{
    float highest_val = 0.0f;

    for (int i = 0; i < continent->num_points; i++)
    {
        struct Point temp = continent->cluster[i];

        double temp_dist = dist_between((struct Pos){x, y}, temp.pos);

        float val = -temp_dist + temp.max_dist;
        val /= temp.max_dist;

        if (val > highest_val)
        {
            highest_val = val;
        }
    }

    return highest_val;
}

