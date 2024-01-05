#include <stdlib.h>

uint8_t *xor_buffers(uint8_t *b1, uint8_t *b2, size_t len)
{
    uint8_t *result = malloc(len);
    for (int i = 0; i < len; i++)
    {
        result[i] = b1[i] ^ b2[i];
    }

    return result;
}