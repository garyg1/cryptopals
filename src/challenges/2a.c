#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../lib/common.c"
#include "../lib/crypto.c"
#include "../lib/hex_to_base64.c"

int main(int argc, char const *argv[])
{
    uint8_t input[] = "YELLOW SUBMARINE";

    size_t padded_len;
    buf_t padded = pad_pkcs7(input, sizeof(input) - 1, 20, &padded_len);

    size_t hex_len;
    char *hex = bytes_to_hex(padded, padded_len, &hex_len);
    printf("%s\n", hex);
}
