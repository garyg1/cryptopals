#include <stdio.h>
#include <stdlib.h>

const char *HEX_LOOKUP = "0123456789abcdef";
const char *BASE64_LOOKUP = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const char BASE64_PADDING = '=';

char to_chr(int v)
{
    return HEX_LOOKUP[v];
}

int to_hex(char c)
{
    if (c >= '0' && c <= '9')
    {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f')
    {
        return 10 + (c - 'a');
    }
    return -1;
}

uint8_t *hex_to_bytes(char *hex, size_t hex_len, size_t *bytes_len)
{
    *bytes_len = (hex_len + 1) / 2;
    uint8_t *bytes = malloc(*bytes_len);
    for (int i = 0; i < hex_len / 2; i++)
    {
        bytes[i] = (to_hex(hex[2 * i]) << 4) | to_hex(hex[2 * i + 1]);
    }

    if (hex_len % 2 == 1)
    {
        bytes[*bytes_len - 1] = to_hex(hex[hex_len - 1]) << 4;
    }

    return bytes;
}

char *bytes_to_hex(uint8_t *bytes, size_t size)
{
    char *hex = malloc(size * 2 * sizeof(char));
    for (size_t i = 0; i < size; i++)
    {
        hex[2 * i] = to_chr((bytes[i] & 0xf0) >> 4);
        hex[2 * i + 1] = to_chr(bytes[i] & 0x0f);
    }

    return hex;
}

char *bytes_to_base64(uint8_t *bytes, size_t bytes_len, size_t *base64_len)
{
    const int padding_case = bytes_len % 3;
    const int padding_len = (padding_case == 0)   ? 0
                            : (padding_case == 1) ? 2
                                                  : 1;
    *base64_len = (bytes_len * 4 + 2) / 3 + padding_len;
    char *base64_result = malloc(*base64_len * sizeof(char));
    for (int i = 0; i < *base64_len - padding_len; i++)
    {
        int bytes_i = i / 4 * 3;
        int base64_idx = 0;
        // https://en.wikipedia.org/wiki/Base64#Examples
        switch (i % 4)
        {
        case 0:
            base64_idx = ((bytes[bytes_i] & 0xfc) >> 2);
            break;
        case 1:
            base64_idx = ((bytes[bytes_i] & 0x03) << 4);
            if (bytes_i + 1 < bytes_len)
            {
                base64_idx |= ((bytes[bytes_i + 1] & 0xf0) >> 4);
            }
            break;
        case 2:
            if (bytes_i + 1 < bytes_len)
            {
                base64_idx |= ((bytes[bytes_i + 1] & 0x0f) << 2);
            }
            if (bytes_i + 2 < bytes_len)
            {
                base64_idx |= ((bytes[bytes_i + 2] & 0xc0) >> 6);
            }
            break;
        case 3:
            if (bytes_i + 2 < bytes_len)
            {
                base64_idx = bytes[bytes_i + 2] & 0x3f;
            }
            break;
        }

        base64_result[i] = BASE64_LOOKUP[base64_idx];
    }

    for (int i = 0; i < padding_len; i++)
    {
        base64_result[*base64_len - 1 - i] = BASE64_PADDING;
    }
    return base64_result;
}