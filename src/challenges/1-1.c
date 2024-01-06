#include <stdio.h>
#include <stdlib.h>
#include "../lib/encoding.c"

int main(int argc, char **argv)
{
    char *input = NULL;
    size_t len;

    int input_len = getline(&input, &len, stdin);
    if (input_len == -1)
    {
        return 1;
    }
    input_len -= 1;

    size_t bytes_len;
    size_t base64_len;

    byte_t *bytes = hex_to_bytes(input, input_len, &bytes_len);
    char *base64 = bytes_to_base64(bytes, bytes_len, &base64_len);
    printf("%s (len=%zu)\n", base64, base64_len);
}