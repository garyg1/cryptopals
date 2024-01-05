#pragma once

#include <stdlib.h>
#include "./common.c"

buf_t xor_buffers(buf_t b1, buf_t b2, size_t len)
{
    buf_t result = malloc(len);
    for (int i = 0; i < len; i++)
    {
        result[i] = b1[i] ^ b2[i];
    }

    return result;
}