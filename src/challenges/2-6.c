#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../lib/common.c"
#include "../lib/crypto.c"
#include "../lib/encoding.c"

buf_t m_key;
const char *unknown_string_b64 = "Um9sbGluJyBpbiBteSA1LjAKV2l0aCBteSByYWctdG9wIGRvd24gc28gbXkgaGFpciBjYW4gYmxvdwpUaGUgZ2lybGllcyBvbiBzdGFuZGJ5IHdhdmluZyBqdXN0IHRvIHNheSBoaQpEaWQgeW91IHN0b3A/IE5vLCBJIGp1c3QgZHJvdmUgYnkK";
buf_t unknown_string;
size_t unknown_string_len;
int oracle_calls = 0;

void init_encryption_oracle()
{
    m_key = random_bytes(16);
    unknown_string = base64_to_bytes(unknown_string_b64, 184, &unknown_string_len);
}

buf_t encryption_oracle(buf_t input, size_t input_len, size_t *enc_len, bool *is_ecb)
{
    oracle_calls += 1;
    size_t rand_len = randrange(0, 100);
    buf_t rand = random_bytes(rand_len);

    buf_t input2 = concat_buffers3(
        rand, rand_len,
        input, input_len,
        unknown_string, unknown_string_len);
    size_t input2_len = rand_len + input_len + unknown_string_len;

    buf_t enc;
    if (rand_bool())
    {
        *is_ecb = false;
        buf_t iv = random_bytes(16);
        enc = encrypt_aes128_cbc(input2, input2_len, m_key, iv, enc_len);
    }
    else
    {
        *is_ecb = true;
        enc = encrypt_aes128_ecb(input2, input2_len, m_key, enc_len);
    }
    return enc;
}

void solve()
{
    byte_t known_plaintext[1000] = {0};
    size_t known_plaintext_len = 0;

    while (known_plaintext_len < unknown_string_len)
    {
        /*
            if (known < 16) {
                ECB(0b | 0b | 0b / known / guess | 0b / known / unknown_byte | ...)
            }
            else {
                offset = 16 - (len(known) % 16) - 1
                ECB(0b | 0b | known[-15:] / guess | offset / ... | known[-15:] / unknown_byte | ...)
            }
        */
        static buf_t zero_block = (buf_t) "\xf0\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0d"
                                          "\xf0\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0d";

        for (int guess = 0; guess < 256;)
        {
            int offset = (known_plaintext_len > 15) ? 15 - (known_plaintext_len % 16) : 0;
            int num_zeros_needed = max(0, 15 - known_plaintext_len);
            int plaintext_start_idx = max(0, known_plaintext_len - 15);
            byte_t guess_byte = (byte_t)guess;
            buf_t block3 = concat_buffers3(
                zero_block, num_zeros_needed,
                known_plaintext + plaintext_start_idx, 15 - num_zeros_needed,
                &guess_byte, 1);

            size_t plaintext_len = 32 + 16 + offset + num_zeros_needed;
            buf_t plaintext = concat_buffers4(
                zero_block, 32,
                block3, 16,
                zero_block, offset,
                zero_block, num_zeros_needed);

            size_t enc_len;
            bool was_ecb;

            buf_t enc = encryption_oracle(plaintext, plaintext_len, &enc_len, &was_ecb);

            int segment_i;
            int segment_j;
            bool is_ecb = try_detect_ecb2(enc, enc_len, &segment_i, &segment_j);

            if (is_ecb)
            {
                int random_offset = segment_i * 16;
                int test_block_idx = random_offset + 48 + 16 * (known_plaintext_len / 16);
                bool is_match = are_bytes_equal(enc + 32 + random_offset, enc + test_block_idx, 16);
                if (is_match)
                {
                    known_plaintext[known_plaintext_len] = guess;
                    known_plaintext_len += 1;
                    break;
                }

                guess += 1;
            }
        }
    }

    printf("Plaintext:\n%s", known_plaintext);
}

int main(int argc, char const *argv[])
{
    srand(time(NULL));
    init_encryption_oracle();
    solve();

    printf("Oracle calls: %d\n", oracle_calls);
}
