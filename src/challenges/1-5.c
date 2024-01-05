#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../lib/hex_to_base64.c"
#include "../lib/english_match.c"

int main(int argc, char const *argv[])
{
    byte_t input1[] = "Burning 'em, if you ain't quick and nimble\nI go crazy when I hear a cymbal";
    byte_t key[] = "ICE";

    buf_t result1 = repeating_key_xor(input1, sizeof(input1) - 1, key, sizeof(key) - 1);
    size_t result1_hex_len;
    char *result1_hex = bytes_to_hex(result1, sizeof(input1) - 1, &result1_hex_len);

    printf("[%zu] %s\n", result1_hex_len, result1_hex);
}