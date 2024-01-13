#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../lib/common.c"
#include "../lib/crypto.c"

static byte_t key[AES_128_BLOCK_SIZE_BYTES];
static byte_t nonce[AES_128_BLOCK_SIZE_BYTES];

void init_encryption_oracle()
{
    random_bytes_inplace(key, AES_128_BLOCK_SIZE_BYTES);
    random_bytes_inplace(nonce, AES_128_BLOCK_SIZE_BYTES);
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

    buf_t enc = calloc(plaintext_len + 1, 1);
    *enc_len = plaintext_len;

    encrypt_aes128_ctr(plaintext, plaintext_len, key, nonce, enc);
    return enc;
}

void decryption_oracle(buf_t ciphertext, size_t ciphertext_len, buf_t dec)
{
    decrypt_aes128_ctr(ciphertext, ciphertext_len, key, nonce, dec);
}

void solve()
{
    size_t input_len = 16;
    byte_t input[] = "\x00\x00\x00\x00\x00\x00\x00\x00"
                     "\x00\x00\x00\x00\x00\x00\x00\x00";

    // bytes 32-47 will be the keystream
    size_t ciphertext_len;
    buf_t ciphertext = encryption_oracle(input, input_len, &ciphertext_len);

    // replace 0000... with admin=true
    xor_buffers_inplace(ciphertext + 32, ciphertext + 32, (buf_t) "aaaaa&admin=true", 16);

    byte_t plaintext[ciphertext_len + 1];
    plaintext[ciphertext_len] = '\0';
    decryption_oracle(ciphertext, ciphertext_len, plaintext);

    printf("%s\n", plaintext);
    free(ciphertext);
}

int main(int argc, char const *argv[])
{
    init_encryption_oracle();
    solve();
}
