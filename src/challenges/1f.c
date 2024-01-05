#include <stdlib.h>
#include <stdio.h>
#include "../lib/edit_distance.c"
#include "../lib/hex_to_base64.c"
#include "../lib/common.c"

int main(int argc, char const *argv[])
{
    unsigned char s1[] = "this is a test";
    unsigned char s2[] = "wokka wokka!!!";
    printf("%d\n", edit_distance(s1, s2, sizeof(s1) - 1));

    char x[] = "AAAATWE==";
    size_t bytes_len;
    buf_t bytes = base64_to_bytes(x, sizeof(x) - 1, &bytes_len);
    printf("%zu\n", bytes_len);

    size_t hex_len;
    char *hex = bytes_to_hex(bytes, bytes_len, &hex_len);
    printf("%s\n", hex);
}
