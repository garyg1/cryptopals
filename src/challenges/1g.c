#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include "../lib/common.c"
#include "../lib/hex_to_base64.c"

int main(int argc, char const *argv[])
{
    size_t base64_len;
    size_t bytes_len;
    char *base64 = read_multiline_from_file("./1-7.txt", &base64_len);
    unsigned char *ciphertext_bytes = (unsigned char *)base64_to_bytes(base64, base64_len, &bytes_len);

    unsigned char key[] = "YELLOW SUBMARINE";
    unsigned char plaintext_bytes = 
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit(ctx, EVP_aes_128_ecb(), key, NULL);
    EVP_DecryptUpdate(ctx, )

}
