#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../lib/common.c"
#include "../lib/crypto.c"
#include "../lib/encoding.c"

static buf_t key;
static buf_t iv;

static buf_t key;
static buf_t iv;

void init_encryption_oracle()
{
    key = random_aes128_key();
    iv = random_aes128_iv();
}

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

buf_t encryption_oracle(size_t *ciphertext_len)
{
    int r = randrange(0, 10);

    size_t plaintext_len;
    buf_t plaintext = base64_to_bytes(strings[r], strlen(strings[r]), &plaintext_len);

    size_t hex_len;
    printf("plaintext: %s\n", bytes_to_hex(plaintext, plaintext_len, &hex_len));

    return encrypt_aes128_cbc(plaintext, plaintext_len, key, iv, ciphertext_len);
}

bool padding_oracle(buf_t ciphertext, size_t ciphertext_len)
{
    size_t plaintext_len;
    buf_t plaintext = decrypt_aes128_cbc(ciphertext, ciphertext_len, key, iv, &plaintext_len);

    size_t hex_len;

    buf_t unpadded;
    size_t unpadded_len;
    bool is_valid = try_unpad_pkcs7(plaintext, plaintext_len, AES_128_BLOCK_SIZE_BYTES, &unpadded, &unpadded_len);

    return is_valid;
}

void do_attack()
{
    size_t ciphertext_len;
    buf_t ciphertext = encryption_oracle(&ciphertext_len);

    buf_t modified_ciphertext = copy_buffer(ciphertext, ciphertext_len);
    // solve last block only
    buf_t known_plaintext = calloc(ciphertext_len + 1, 1);
    known_plaintext[ciphertext_len] = '\0';
    size_t known_plaintext_len = 0;
    buf_t known_guesses = calloc(ciphertext_len + 1, 1);
    known_guesses[ciphertext_len] = '\0';
    size_t discard;
    size_t block_size = AES_128_BLOCK_SIZE_BYTES;
    for (int ciphertext_idx = ciphertext_len - 1; ciphertext_idx >= ciphertext_len - 16; ciphertext_idx--)
    {
        for (uint guess_byte = 0; guess_byte < 256; guess_byte++)
        {
            modified_ciphertext[ciphertext_idx - block_size] = guess_byte;
            byte_t target_byte = ciphertext_len - ciphertext_idx;

            if (padding_oracle(modified_ciphertext, ciphertext_len))
            {
                byte_t known_byte = ciphertext[ciphertext_idx - block_size] ^ guess_byte ^ target_byte;
                // guess = target ^ known ^ ciphertext
                if (known_byte == 0x01)
                {
                    continue;
                }
                known_plaintext[ciphertext_idx] = known_byte;
                known_guesses[ciphertext_idx] = guess_byte;
                known_plaintext_len += 1;
                printf("byte: %zu\n", known_plaintext[ciphertext_idx]);
                // printf("prev guess: %d\n", guess_byte);
                break;
            }
        }

        for (int i = 0; i < known_plaintext_len; i++)
        {
            byte_t guess2 = known_guesses[ciphertext_idx + i] ^ (known_plaintext_len - i) ^ (known_plaintext_len + 1);
            // printf("guess2: %d\n", guess2);
            modified_ciphertext[ciphertext_idx + i - block_size] = guess2;
        }
    }

    printf("%s\n", bytes_to_hex(known_plaintext + (ciphertext_len - 16), 16, &discard));
}

int main(int argc, char const *argv[])
{
    srand(time(NULL));
    init_encryption_oracle();
    do_attack();
}
