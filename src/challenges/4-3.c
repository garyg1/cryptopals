#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../lib/common.c"
#include "../lib/crypto.c"

static byte_t key[AES_128_BLOCK_SIZE_BYTES];

void init_encryption_oracle()
{
    random_bytes_inplace(key, AES_128_BLOCK_SIZE_BYTES);
}

buf_t encryption_oracle(buf_t input, size_t input_len, size_t *enc_len)
{
    const buf_t prefix = (buf_t) "comment1=cooking%20MCs&userdata=";
    const size_t prefix_len = 32;
    const buf_t suffix = (buf_t) "&comment2=%20like%20a%20pound%20of%20bacon";
    const size_t suffix_len = 42;

    byte_t plaintext[prefix_len + input_len + suffix_len];
    size_t plaintext_len = prefix_len + input_len + suffix_len;
    concat_buffers3_inplace(
        plaintext,
        prefix, prefix_len,
        input, input_len,
        suffix, suffix_len);

    buf_t enc = encrypt_aes128_cbc(plaintext, plaintext_len, key, key, enc_len);
    return enc;
}

buf_t decryption_oracle(buf_t ciphertext, size_t ciphertext_len, size_t *dec_len)
{
    return decrypt_aes128_cbc(ciphertext, ciphertext_len, key, key, dec_len);
}

void solve()
{
    size_t input_len = 16;
    byte_t input[] = "\x00\x00\x00\x00\x00\x00\x00\x00"
                     "\x00\x00\x00\x00\x00\x00\x00\x00";

    size_t ciphertext_len;
    buf_t ciphertext = encryption_oracle(input, input_len, &ciphertext_len);

    memset(ciphertext + 16, 0, 16);
    memcpy(ciphertext + 32, ciphertext, 16);

    size_t plaintext_len;
    buf_t plaintext = decryption_oracle(ciphertext, ciphertext_len, &plaintext_len);

    byte_t key_guess[16];
    xor_buffers_inplace(key_guess, plaintext, plaintext + 32, 16);

    size_t discard;
    char *key_hex = bytes_to_hex(key_guess, 16, &discard);
    char *key_guess_hex = bytes_to_hex(key, 16, &discard);
    printf("actual key: %s\n", key_hex);
    printf("recovered key: %s\n", key_guess_hex);

    free(ciphertext);
    free(plaintext);
    free(key_hex);
    free(key_guess_hex);
}

int main(int argc, char const *argv[])
{
    init_encryption_oracle();
    solve();
}
