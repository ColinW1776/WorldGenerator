unsigned char base_permutations[]=
{
    151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
    8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
    35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,
    134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
    55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,18,
    169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,250,
    124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,
    8,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,172,9,
    129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,228,
    251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,
    49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,
    138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

#define FASTFLOOR(x) ( ((int)(x)<=(x)) ? ((int)x) : (((int)x)-1) )

float grad2( int hash, float x, float y )
{
    int h = hash & 7;      // Convert low 3 bits of hash code
    float u = h<4 ? x : y;  // into 8 simple gradient directions,
    float v = h<4 ? y : x;  // and compute the dot product with (x,y).
    return ((h&1)? -u : u) + ((h&2)? -2.0f*v : 2.0f*v);
}

float noise(float x, float y, unsigned char* perm)
{
    #define F2 0.366025403 // F2 = 0.5*(sqrt(3.0)-1.0)
    #define G2 0.211324865 // G2 = (3.0-Math.sqrt(3.0))/6.0

    float n0, n1, n2; // Noise contributions from the three corners

    // Skew the input space to determine which simplex cell we're in
    float s = (x+y)*F2; // Hairy factor for 2D
    float xs = x + s;
    float ys = y + s;
    int i = FASTFLOOR(xs);
    int j = FASTFLOOR(ys);

    float t = (float)(i+j)*G2;
    float X0 = i-t; // Unskew the cell origin back to (x,y) space
    float Y0 = j-t;
    float x0 = x-X0; // The x,y distances from the cell origin
    float y0 = y-Y0;

    // For the 2D case, the simplex shape is an equilateral triangle.
    // Determine which simplex we are in.
    int i1, j1; // Offsets for second (middle) corner of simplex in (i,j) coords
    if(x0>y0) {i1=1; j1=0;} // lower triangle, XY order: (0,0)->(1,0)->(1,1)
    else {i1=0; j1=1;}      // upper triangle, YX order: (0,0)->(0,1)->(1,1)

    // A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
    // a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
    // c = (3-sqrt(3))/6

    float x1 = x0 - i1 + G2; // Offsets for middle corner in (x,y) unskewed coords
    float y1 = y0 - j1 + G2;
    float x2 = x0 - 1.0f + 2.0f * G2; // Offsets for last corner in (x,y) unskewed coords
    float y2 = y0 - 1.0f + 2.0f * G2;

    // Wrap the integer indices at 256, to avoid indexing perm[] out of bounds
    int ii = i & 0xff;
    int jj = j & 0xff;

    // Calculate the contribution from the three corners
    float t0 = 0.5f - x0*x0-y0*y0;
    if(t0 < 0.0f) n0 = 0.0f;
    else {
        t0 *= t0;
        n0 = t0 * t0 * grad2(perm[ii+perm[jj]], x0, y0);
    }

    float t1 = 0.5f - x1*x1-y1*y1;
    if(t1 < 0.0f) n1 = 0.0f;
    else {
        t1 *= t1;
        n1 = t1 * t1 * grad2(perm[ii+i1+perm[jj+j1]], x1, y1);
    }

    float t2 = 0.5f - x2*x2-y2*y2;
    if(t2 < 0.0f) n2 = 0.0f;
    else {
        t2 *= t2;
        n2 = t2 * t2 * grad2(perm[ii+1+perm[jj+1]], x2, y2);
    }

    // Add contributions from each corner to get the final noise value.
    // The result is scaled to return values in the interval [-1,1].
    return 40.0f * (n0 + n1 + n2); // TODO: The scale factor is preliminary!
}

float octave(float x, float y, int octaves, unsigned char* perm)
{
    float total = 0;
    float max = 0;
    float amp = 1;
    float freq = 1;

    for (int i = 0; i < octaves; i++)
    {
        total += noise(x * freq, y * freq, perm);
        max += amp;

        amp *= 2;
        freq /= 2;
    }

    return total / max;
}

unsigned char* get_permutations(int64_t seed)
{
    srand(seed);

    unsigned char* perms = malloc(512);

    for (int i = 0; i < 512; i++)
    {
        perms[i] = base_permutations[i%256];
    }

    for (int i = 512 -1; i > 0; i--)
    {
        int index = rand();
        index %= (i + 1);

        unsigned char val = perms[index];
        perms[index] = perms[i];
        perms[i] = val;
    }

    return perms;
}

void simplex(float* values, int octaves, int size, int64_t seed)
{
    unsigned char* perms = get_permutations(seed);

    for (int x = 0; x < size; x++)
    {
        for (int y = 0; y < size; y++)
        {
            values[x * size + y] = ((octave((float)x / 72.0f, (float)y / 72.0f, octaves, perms)) + 1) / 2;
            // go back to 60
        }
    }
}

void simplex_image(float* values, int size)
{
    unsigned char* pixels = malloc(3 * size * size);
    unsigned int index = 0;

    for (int y = 0; y < size; y++)
    {
        for (int x = 0; x < size; x++)
        {
            unsigned char color = (unsigned char)(values[x * size + y] * 255);

            pixels[index] = color;
            pixels[index + 1] = color;
            pixels[index + 2] = color;

            index += 3;
        }
    }

    stbi_write_png("simplex.png", size, size, 3, pixels, 3 * size);
    free(pixels);
}