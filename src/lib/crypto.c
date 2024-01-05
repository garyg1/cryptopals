#pragma once
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "./common.c"

buf_t pad_pkcs7(buf_t buf, size_t buf_len, size_t block_size, size_t *padded_len)
{
    int padding_len = block_size - (buf_len % block_size);
    *padded_len = buf_len + padding_len;

    buf_t out = malloc(*padded_len);
    printf("%zu %d\n", *padded_len, padding_len);
    memcpy(out, buf, buf_len);
    fill_byte(out + buf_len, (uint8_t)padding_len, padding_len);

    return out;
}