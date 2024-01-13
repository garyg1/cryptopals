#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include "../lib/mersenne.c"
#include "../lib/common.c"
#include "../lib/encoding.c"

void encrypt_mt(buf_t plaintext, size_t plaintext_len, uint16_t seed, buf_t ciphertext)
{
    struct mersenne_state state;
    mt_seed(&state, seed);

    for (size_t i = 0; i < plaintext_len; i++)
    {
        long byte = mt_random(&state);

        ciphertext[i] = plaintext[i] ^ byte;
    }
}

void decrypt_mt(buf_t ciphertext, size_t ciphertext_len, uint16_t seed, buf_t plaintext)
{
    struct mersenne_state state;
    mt_seed(&state, seed);

    for (size_t i = 0; i < ciphertext_len; i++)
    {
        long byte = mt_random(&state);

        plaintext[i] = ciphertext[i] ^ byte;
    }
}

void test_encrypt_decrypt()
{
    byte_t plaintext[45] = "The quick brown fox jumps over the lazy dog.";
    byte_t ciphertext[45];

    uint16_t seed = 0xffff;
    encrypt_mt(plaintext, 44, seed, ciphertext);

    byte_t plaintext2[45];
    decrypt_mt(ciphertext, 44, seed, plaintext2);
    plaintext2[44] = '\0';

    printf("%s\n", plaintext);
    printf("%s\n", plaintext2);
}

void search_for_seed()
{
    const int max_seed = 0xffff;
    int rand_seed = randrange(0, max_seed);

    printf("actual seed: %d\n", rand_seed);

    int num_rand_bytes = randrange(10, 100);
    byte_t rand[num_rand_bytes];
    random_bytes_inplace(rand, num_rand_bytes);

    byte_t known_suffix[16] = "AAAAAAAAAAAAAAAA";
    buf_t plaintext = concat_buffers2(rand, num_rand_bytes, known_suffix, 16);
    int plaintext_len = num_rand_bytes + 16;

    byte_t ciphertext[plaintext_len];
    encrypt_mt(plaintext, plaintext_len, rand_seed, ciphertext);
    int ciphertext_len = plaintext_len;

    byte_t plaintext_guess[plaintext_len];
    for (int seed_guess = 0; seed_guess < max_seed; seed_guess++)
    {
        decrypt_mt(ciphertext, ciphertext_len, seed_guess, plaintext_guess);
        if (contains_substring(plaintext_guess, plaintext_len, known_suffix, 8))
        {
            printf("seed guess: %d\n", seed_guess);
            break;
        }
    }

    free(plaintext);
}

int main(int argc, char const *argv[])
{
    srand(time(NULL));
    test_encrypt_decrypt();
    search_for_seed();
}
