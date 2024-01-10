#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../lib/common.c"
#include "../lib/crypto.c"
#include "../lib/encoding.c"

int main(int argc, char const *argv[])
{
    const char *base64 = "L77na/nrFsKvynd6HzOoG7GHTLXsTVu9qvY/2syLXzhPweyyMTJULu/6/kXX0KSvoOLSFQ==";

    size_t ciphertext_len;
    buf_t ciphertext = base64_to_bytes(base64, 72, &ciphertext_len);

    buf_t key = (buf_t) "YELLOW SUBMARINE";
    buf_t nonce = (buf_t)ZERO_BUFFER_16;

    byte_t plaintext[ciphertext_len + 1];
    plaintext[ciphertext_len] = '\0';
    decrypt_aes128_ctr(ciphertext, ciphertext_len, key, nonce, plaintext);
    printf("%s\n", plaintext);

    // encrypt/decrypt again to validate
    byte_t ciphertext2[ciphertext_len];
    encrypt_aes128_ctr(plaintext, ciphertext_len, key, nonce, ciphertext2);

    byte_t plaintext2[ciphertext_len];
    decrypt_aes128_ctr(ciphertext2, ciphertext_len, key, nonce, plaintext2);
    printf("%s\n", plaintext2);
}
