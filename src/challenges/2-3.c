#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../lib/common.c"
#include "../lib/crypto.c"
#include "../lib/encoding.c"

buf_t key;

void init_encryption_oracle()
{
    key = random_bytes(16);
}

buf_t encryption_oracle(buf_t input, size_t input_len, size_t *enc_len, bool *is_ecb)
{
    size_t num_bytes_before = randrange(5, 11);
    size_t num_bytes_after = randrange(5, 11);
    buf_t bytes_before = random_bytes(num_bytes_before);
    buf_t bytes_afer = random_bytes(num_bytes_after);

    buf_t input2 = concat_buffers3(bytes_before, num_bytes_before, input, input_len, bytes_afer, num_bytes_after);
    size_t input2_len = num_bytes_before + input_len + num_bytes_after;

    buf_t enc;
    if (rand_bool())
    {
        *is_ecb = false;
        buf_t iv = random_bytes(16);
        enc = encrypt_aes128_cbc(input2, input2_len, key, iv, enc_len);
    }
    else
    {
        *is_ecb = true;
        enc = encrypt_aes128_ecb(input2, input2_len, key, enc_len);
    }
    return enc;
}

int main(int argc, char const *argv[])
{
    srand(time(NULL));
    init_encryption_oracle();

    const int NUM_TESTS = 10000;
    int successes = 0;
    for (int i = 0; i < NUM_TESTS; i++)
    {
        size_t bytes_len = 48;
        buf_t bytes = (buf_t) "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";

        size_t enc_len;
        bool was_ecb;
        buf_t enc = encryption_oracle(bytes, bytes_len, &enc_len, &was_ecb);
        bool is_ecb = try_detect_ecb(enc, enc_len);

        if (is_ecb != was_ecb)
        {
            printf("Fail - is_ecb: %d was_ecb: %d\n", is_ecb, was_ecb);
        }
        else
        {
            successes += 1;
        }
    }
    printf("Successes: %d / %d\n", successes, NUM_TESTS);
}
