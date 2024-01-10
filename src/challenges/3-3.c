#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../lib/common.c"
#include "../lib/crypto.c"
#include "../lib/encoding.c"

int main(int argc, char const *argv[])
{
    size_t num_lines;
    char **lines = read_lines_from_file("./3-3.txt", &num_lines);

    byte_t key[AES_128_BLOCK_SIZE_BYTES];
    random_aes128_key_inplace(key);

    buf_t nonce = (buf_t)ZERO_BUFFER_16;

    buf_t ciphertexts[num_lines];
    size_t ciphertext_lens[num_lines];

    for (int i = 0; i < num_lines; i++)
    {
        char *line = lines[i];

        size_t plaintext_len;
        buf_t plaintext = base64_to_bytes(line, strlen(line), &plaintext_len);

        buf_t ciphertext = calloc(plaintext_len, 1);
        encrypt_aes128_ctr(plaintext, plaintext_len, key, nonce, ciphertext);

        ciphertexts[i] = ciphertext;
        ciphertext_lens[i] = plaintext_len;

        free(plaintext);
    }

    for (int i = 0; i < num_lines; i++)
    {
        size_t discard;
        printf("%s\n", bytes_to_hex(ciphertexts[i], ciphertext_lens[i], &discard));

        // todo: solve
        byte_t plaintext[ciphertext_lens[i] + 1];
        decrypt_aes128_ctr(ciphertexts[i], ciphertext_lens[i], key, nonce, plaintext);
        plaintext[ciphertext_lens[i]] = '\0';
        printf("%s\n", plaintext);
    }
}
