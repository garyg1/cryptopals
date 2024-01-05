#pragma once
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <openssl/aes.h>
#include <openssl/ssl.h>
#include "./common.c"

buf_t pad_pkcs7(buf_t buf, size_t buf_len, size_t block_size, size_t *padded_len)
{
    int padding_len = block_size - (buf_len % block_size);
    *padded_len = buf_len + padding_len;

    buf_t out = malloc(*padded_len);
    printf("%zu %d\n", *padded_len, padding_len);
    memcpy(out, buf, buf_len);
    fill_byte(out + buf_len, (byte_t)padding_len, padding_len);

    return out;
}

const size_t AES_128_BLOCK_SIZE_BYTES = 16;
const size_t AES_128_KEY_SIZE_BYTES = 16;

buf_t decrypt_block_aes128(buf_t buf, buf_t key)
{
    AES_KEY enc_key;
    AES_set_decrypt_key(key, AES_128_KEY_SIZE_BYTES * 8, &enc_key);

    buf_t out = calloc(AES_128_BLOCK_SIZE_BYTES, 1);
    AES_decrypt(buf, out, &enc_key);

    return out;
}

buf_t decrypt_aes128_cbc(buf_t ciphertext, size_t ciphertext_len, buf_t key, buf_t iv, size_t *plaintext_len)
{
    int num_blocks = ciphertext_len / AES_128_BLOCK_SIZE_BYTES;

    buf_t prev = iv;
    buf_t out = calloc(num_blocks * AES_128_BLOCK_SIZE_BYTES, 1);
    for (int i = 0; i < num_blocks; i++)
    {
        buf_t curr_ciphertext_block = ciphertext + i * AES_128_BLOCK_SIZE_BYTES;
        buf_t dec_block = decrypt_block_aes128(curr_ciphertext_block, key);
        buf_t plaintext_block = xor_buffers(dec_block, prev, AES_128_BLOCK_SIZE_BYTES);
        memcpy(out + i * AES_128_BLOCK_SIZE_BYTES, plaintext_block, AES_128_BLOCK_SIZE_BYTES);

        prev = curr_ciphertext_block;
    }

    *plaintext_len = ciphertext_len;
    return out;
}