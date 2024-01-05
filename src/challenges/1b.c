#include <stdio.h>
#include <stdlib.h>
#include "../lib/hex_to_base64.c"
#include "../lib/xor_buffers.c"

typedef uint8_t *buf_t;

int main(int argc, char const *argv[])
{
    buf_t buf1, buf2;
    size_t buf1_len, buf2_len;
    buf1 = hex_to_bytes("1c0111001f010100061a024b53535009181c", 36, &buf1_len);
    buf2 = hex_to_bytes("686974207468652062756c6c277320657965", 36, &buf2_len);

    buf_t buf3 = xor_buffers(buf1, buf2, buf1_len);

    size_t buf3_hex_len;
    char *buf3_hex = bytes_to_hex(buf3, buf1_len, &buf3_hex_len);
    printf("%s\n", buf3_hex);
}
