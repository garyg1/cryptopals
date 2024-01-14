#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../lib/common.c"
#include "../lib/crypto.c"
#include "../lib/clibs_sha1.c"

int main(int argc, char const *argv[])
{
    byte_t key[] = "YELLOW SUBMARINE";
    size_t key_len = sizeof(key) - 1;

    byte_t message[] = "the quick brown fox jumps over the lazy dog.";
    size_t message_len = sizeof(message) - 1;

    size_t input_len = key_len + message_len;
    byte_t input[input_len];
    concat_buffers2_inplace(input, key, key_len, message, message_len);

    printf("%s\n", input);

    byte_t hash_out[20];
    clibs_SHA1(hash_out, input, input_len);

    size_t discard;
    char *base64 = bytes_to_base64(hash_out, 20, &discard);
    printf("%s\n", base64);

    free(base64);
}