#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../lib/common.c"
#include "../lib/crypto.c"
#include "../lib/encoding.c"

const char *strings[] = {
    "MDAwMDAwTm93IHRoYXQgdGhlIHBhcnR5IGlzIGp1bXBpbmc=",
    "MDAwMDAxV2l0aCB0aGUgYmFzcyBraWNrZWQgaW4gYW5kIHRoZSBWZWdhJ3MgYXJlIHB1bXBpbic=",
    "MDAwMDAyUXVpY2sgdG8gdGhlIHBvaW50LCB0byB0aGUgcG9pbnQsIG5vIGZha2luZw==",
    "MDAwMDAzQ29va2luZyBNQydzIGxpa2UgYSBwb3VuZCBvZiBiYWNvbg==",
    "MDAwMDA0QnVybmluZyAnZW0sIGlmIHlvdSBhaW4ndCBxdWljayBhbmQgbmltYmxl",
    "MDAwMDA1SSBnbyBjcmF6eSB3aGVuIEkgaGVhciBhIGN5bWJhbA==",
    "MDAwMDA2QW5kIGEgaGlnaCBoYXQgd2l0aCBhIHNvdXBlZCB1cCB0ZW1wbw==",
    "MDAwMDA3SSdtIG9uIGEgcm9sbCwgaXQncyB0aW1lIHRvIGdvIHNvbG8=",
    "MDAwMDA4b2xsaW4nIGluIG15IGZpdmUgcG9pbnQgb2g=",
    "MDAwMDA5aXRoIG15IHJhZy10b3AgZG93biBzbyBteSBoYWlyIGNhbiBibG93",
};

const size_t BLOCK_SIZE = AES_128_BLOCK_SIZE_BYTES;
static buf_t m_key;

void init_encryption_oracle()
{
    m_key = random_aes128_key();
}

void encryption_oracle(buf_t *ciphertext, size_t *ciphertext_len, buf_t *iv, int i)
{
    size_t plaintext_len;
    buf_t plaintext = base64_to_bytes(strings[i], strlen(strings[i]), &plaintext_len);

    *iv = random_aes128_iv();
    *ciphertext = encrypt_aes128_cbc(plaintext, plaintext_len, m_key, *iv, ciphertext_len);

    free(plaintext);
}

bool padding_oracle(const buf_t ciphertext, size_t ciphertext_len, const buf_t iv)
{
    size_t plaintext_len;
    buf_t plaintext = decrypt_aes128_cbc(ciphertext, ciphertext_len, m_key, iv, &plaintext_len);

    buf_t unpadded = NULL;
    size_t unpadded_len;
    bool is_valid = try_unpad_pkcs7(plaintext, plaintext_len, AES_128_BLOCK_SIZE_BYTES, &unpadded, &unpadded_len);

    free(plaintext);
    if (unpadded != NULL)
    {
        free(unpadded);
    }

    return is_valid;
}

buf_t decode_one_block(const buf_t ciphertext, size_t ciphertext_len, const buf_t iv)
{
    size_t known_plaintext_len = 0;
    buf_t known_plaintext = calloc(BLOCK_SIZE + 1, 1);
    known_plaintext[BLOCK_SIZE] = '\0';

    buf_t known_guesses = calloc(ciphertext_len + 1, 1);
    known_guesses[ciphertext_len] = '\0';

    size_t ciphertext_and_iv_len = BLOCK_SIZE + ciphertext_len;
    buf_t ciphertext_and_iv = concat_buffers2(
        iv, BLOCK_SIZE,
        ciphertext, ciphertext_len);
    buf_t modified_ciphertext_and_iv = copy_buffer(ciphertext_and_iv, ciphertext_and_iv_len);

    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        int ciphertext_idx = ciphertext_len - i - 1;
        for (int guess_byte = 0; guess_byte < 256; guess_byte++)
        {
            modified_ciphertext_and_iv[ciphertext_idx] = guess_byte;
            byte_t target_byte = ciphertext_len - ciphertext_idx;

            if (padding_oracle(modified_ciphertext_and_iv + BLOCK_SIZE, ciphertext_len, modified_ciphertext_and_iv))
            {
                byte_t known_byte = ciphertext_and_iv[ciphertext_idx] ^ guess_byte ^ target_byte;
                // if first byte there are two matches (1) 0x01 and (2) the existing padding
                if (i == 0 && known_byte == 0x01)
                {
                    known_byte = target_byte;
                }

                known_plaintext[BLOCK_SIZE - i - 1] = known_byte;
                known_guesses[ciphertext_idx] = guess_byte;
                known_plaintext_len += 1;
                break;
            }
        }

        // set masked guesses to new target suffix (e.g., \x04\x04\x04)
        for (int i = 0; i < known_plaintext_len; i++)
        {
            byte_t old_target = known_plaintext_len - i;
            byte_t new_target = known_plaintext_len + 1;
            byte_t new_guess = known_guesses[ciphertext_idx + i] ^ old_target ^ new_target;
            modified_ciphertext_and_iv[BLOCK_SIZE + ciphertext_idx + i - BLOCK_SIZE] = new_guess;
        }
    }

    free(known_guesses);
    free(ciphertext_and_iv);
    free(modified_ciphertext_and_iv);

    return known_plaintext;
}

void do_attack()
{
    for (int test_case = 0; test_case < 10; test_case++)
    {
        size_t ciphertext_len;
        buf_t ciphertext;
        buf_t iv;

        encryption_oracle(&ciphertext, &ciphertext_len, &iv, test_case);

        size_t plaintext_len = 0;
        buf_t plaintext = calloc(ciphertext_len, 1);

        for (int i = BLOCK_SIZE; i <= ciphertext_len; i += BLOCK_SIZE)
        {
            buf_t plaintext_chunk = decode_one_block(ciphertext, i, iv);
            memcpy(plaintext + plaintext_len, plaintext_chunk, BLOCK_SIZE);
            plaintext_len += BLOCK_SIZE;

            free(plaintext_chunk);
        }

        buf_t unpadded_plaintext = NULL;
        size_t unpadded_plaintext_len;
        try_unpad_pkcs7(plaintext, plaintext_len, BLOCK_SIZE, &unpadded_plaintext, &unpadded_plaintext_len);

        printf("[%d] %s\n", test_case, unpadded_plaintext);

        free(plaintext);
        free(ciphertext);
        free(iv);
        if (unpadded_plaintext != NULL)
        {
            free(unpadded_plaintext);
        }
    }
}

int main(int argc, char const *argv[])
{
    srand(time(NULL));
    init_encryption_oracle();
    do_attack();
}
