#include <stdlib.h>
#include "./common.c"
#include <stdio.h>

int count_ones(uint8_t byte)
{
    static const int NIBBLE_LOOKUP[16] =
        {
            0, 1, 1, 2, 1, 2, 2, 3,
            1, 2, 2, 3, 2, 3, 3, 4};

    return NIBBLE_LOOKUP[byte & 0x0f] + NIBBLE_LOOKUP[(byte >> 4) & 0x0f];
}

int edit_distance(buf_t b1, buf_t b2, size_t len)
{
    int distance = 0;
    for (size_t i = 0; i < len; i++)
    {
        uint8_t chr = b1[i] ^ b2[i];
        
        int num_ones = count_ones(chr);
        distance += num_ones;
    }

    return distance;
}