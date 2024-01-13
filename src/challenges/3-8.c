#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include "../lib/mersenne.c"
#include "../lib/common.c"
#include "../lib/encoding.c"

const int MAX_SEED = 0xffff;

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

    assert(are_bytes_equal(plaintext, plaintext2, 44));
}

void known_plaintext_attack()
{
    int actual_seed = randrange(0, MAX_SEED);
    printf("actual seed: %d\n", actual_seed);

    int num_rand_bytes = randrange(10, 100);
    byte_t rand[num_rand_bytes];
    random_bytes_inplace(rand, num_rand_bytes);

    byte_t known_suffix[16] = "AAAAAAAAAAAAAAAA";
    buf_t plaintext = concat_buffers2(rand, num_rand_bytes, known_suffix, 16);
    int plaintext_len = num_rand_bytes + 16;

    byte_t ciphertext[plaintext_len];
    encrypt_mt(plaintext, plaintext_len, actual_seed, ciphertext);
    int ciphertext_len = plaintext_len;

    byte_t plaintext_guess[plaintext_len];
    for (int seed_guess = 0; seed_guess < MAX_SEED; seed_guess++)
    {
        decrypt_mt(ciphertext, ciphertext_len, seed_guess, plaintext_guess);
        if (contains_substring(plaintext_guess, plaintext_len, known_suffix, 8))
        {
            printf("seed guess: %d\n", seed_guess);
            assert(actual_seed == seed_guess);
            break;
        }
    }

    printf("known_plaintext_attack - PASSED\n");
    free(plaintext);
}

void make_password_token(buf_t buf, size_t buf_len)
{
    long seed = time(NULL);
    struct mersenne_state state;
    mt_seed(&state, seed);

    for (int i = 0; i < buf_len; i++)
    {
        buf[i] = mt_random(&state) & 0xff;
    }
}

bool is_prng_generated(buf_t buf, size_t buf_len)
{
    struct mersenne_state state;
    long seed_guess_lo = time(NULL) - 10000;
    long seed_guess_hi = time(NULL);
    for (long seed_guess = seed_guess_lo; seed_guess <= seed_guess_hi; seed_guess++)
    {
        mt_seed(&state, seed_guess);
        bool different = false;
        for (int i = 0; i < buf_len; i++)
        {
            byte_t byte_guess = mt_random(&state) & 0xff;
            if (byte_guess != buf[i])
            {
                different = true;
                break;
            }
        }

        if (!different)
        {
            return true;
        }
    }

    return false;
}

void known_ciphertext_and_rough_seed_attack()
{
    byte_t password_token[16];
    make_password_token(password_token, 16);

    byte_t rand_bytes[16];
    random_bytes_inplace(rand_bytes, 16);

    assert(is_prng_generated(password_token, 16));
    assert(!is_prng_generated(rand_bytes, 16));

    printf("known_ciphertext_and_rough_seed_attack - PASSED\n");
}

int main(int argc, char const *argv[])
{
    srand(time(NULL));
    test_encrypt_decrypt();
    known_plaintext_attack();
    known_ciphertext_and_rough_seed_attack();
}
