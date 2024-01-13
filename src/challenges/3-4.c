#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../lib/common.c"
#include "../lib/crypto.c"
#include "../lib/encoding.c"
#include "../lib/english_match.c"

// 3-4 and 3-3 are same
int main(int argc, char const *argv[])
{
    size_t num_lines;
    char **lines = read_lines_from_file("./3-4.txt", &num_lines);

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

    int min_len = INT_MAX;
    for (int i = 0; i < num_lines; i++)
    {
        min_len = min(min_len, ciphertext_lens[i]);
    }

    byte_t x[1000] = {0};
    byte_t guesses[1000] = {0};
    for (int char_idx = 0; char_idx < min_len; char_idx++)
    {
        int best_score = 0;
        int best_guess = 0;
        for (int guess = 0; guess < 256; guess++)
        {
            for (int ciphertext_idx = 0; ciphertext_idx < num_lines; ciphertext_idx++)
            {
                x[ciphertext_idx] = ciphertexts[ciphertext_idx][char_idx] ^ guess;
            }

            int score = english_score((char *)x, num_lines);
            if (score > best_score)
            {
                best_guess = guess;
                best_score = score;
            }
        }

        guesses[char_idx] = best_guess;
    }

    byte_t out[1000] = {0};
    for (int i = 0; i < num_lines; i++)
    {
        xor_buffers_inplace(out, ciphertexts[i], guesses, min_len);
        out[min_len] = '\0';

        printf("[%02d] %s\n", i, out);
    }
}
