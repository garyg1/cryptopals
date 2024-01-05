#include <stdlib.h>
#include "./common.c"

uint8_t count_ones(uint8_t byte)
{
    static const uint8_t NIBBLE_LOOKUP[16] =
        {
            0, 1, 1, 2, 1, 2, 2, 3,
            1, 2, 2, 3, 2, 3, 3, 4};

    return NIBBLE_LOOKUP[byte & 0x0F] + NIBBLE_LOOKUP[byte >> 4];
}

int edit_distance(buf_t b1, buf_t b2, size_t len)
{
    int distance = 0;
    for (size_t i = 0; i < len; i++)
    {
        uint8_t chr = b1[i] ^ b2[i];
        distance += count_ones(chr);
    }

    return distance;
}