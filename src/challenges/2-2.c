#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../lib/common.c"
#include "../lib/crypto.c"
#include "../lib/hex_to_base64.c"

int main(int argc, char const *argv[])
{
    byte_t key[] = "YELLOW SUBMARINE";
    byte_t iv[] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

    size_t ciphertext_b64_len;
    char *ciphertext_b64 = read_multiline_from_file("./2-2.txt", &ciphertext_b64_len);

    size_t bytes_len;
    buf_t bytes = base64_to_bytes(ciphertext_b64, ciphertext_b64_len, &bytes_len);

    size_t enc_len;
    buf_t enc = decrypt_aes128_cbc(bytes, bytes_len, key, iv, &enc_len);

    printf("%s\n", enc);
}
