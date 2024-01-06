#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include "../lib/common.c"
#include "../lib/encoding.c"

int main(int argc, char const *argv[])
{
    size_t base64_len;
    size_t ciphertext_len;
    char *base64 = read_multiline_from_file("./1-7.txt", &base64_len);
    printf("%s\n", base64);
    buf_t bytes = base64_to_bytes(base64, base64_len, &ciphertext_len);
    unsigned char *ciphertext = (unsigned char *)bytes;

    unsigned char key[] = "YELLOW SUBMARINE";
    int plaintext_len;
    unsigned char plaintext[ciphertext_len];
    printf("Making OpenSSL context\n");
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

    printf("Calling EVP_DecryptInit\n");
    EVP_DecryptInit(ctx, EVP_aes_128_ecb(), key, NULL);

    printf("Calling EVP_DecryptUpdate\n");
    EVP_DecryptUpdate(ctx, plaintext, &plaintext_len, ciphertext, (int)ciphertext_len + 1);
    plaintext[plaintext_len] = '\0';

    printf("plaintext: %s\n", plaintext);
}
