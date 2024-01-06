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
    memcpy(out, buf, buf_len);
    fill_byte(out + buf_len, (byte_t)padding_len, padding_len);

    return out;
}

buf_t unpad_pkcs7(buf_t buf, size_t buf_len, size_t block_size, size_t *unpadded_len)
{
    size_t num_chars = (size_t)buf[buf_len - 1];
    *unpadded_len = buf_len - num_chars;
    return buffer_substring(buf, buf_len - num_chars);
}

const size_t AES_128_BLOCK_SIZE_BYTES = 16;
const size_t AES_128_KEY_SIZE_BYTES = 16;

buf_t decrypt_block_aes128(buf_t buf, buf_t key)
{
    AES_KEY dec_key;
    AES_set_decrypt_key(key, AES_128_KEY_SIZE_BYTES * 8, &dec_key);

    buf_t out = calloc(AES_128_BLOCK_SIZE_BYTES, 1);
    AES_decrypt(buf, out, &dec_key);

    return out;
}

buf_t encrypt_block_aes128(buf_t buf, buf_t key)
{
    AES_KEY enc_key;
    AES_set_encrypt_key(key, AES_128_KEY_SIZE_BYTES * 8, &enc_key);

    buf_t out = calloc(AES_128_BLOCK_SIZE_BYTES, 1);
    AES_encrypt(buf, out, &enc_key);

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

buf_t decrypt_aes128_cbc_unpad(buf_t ciphertext, size_t ciphertext_len, buf_t key, buf_t iv, size_t *plaintext_len)
{
    size_t padded_len;
    buf_t padded = decrypt_aes128_cbc(ciphertext, ciphertext_len, key, iv, &padded_len);

    return unpad_pkcs7(padded, padded_len, AES_128_BLOCK_SIZE_BYTES, plaintext_len);
}

buf_t encrypt_aes128_cbc(buf_t unpadded_plaintext, size_t unpadded_plaintext_len, buf_t key, buf_t iv, size_t *ciphertext_len)
{
    size_t plaintext_len;
    buf_t plaintext = pad_pkcs7(unpadded_plaintext, unpadded_plaintext_len, AES_128_BLOCK_SIZE_BYTES, &plaintext_len);
    int num_blocks = plaintext_len / AES_128_BLOCK_SIZE_BYTES;

    buf_t prev = iv;
    buf_t out = calloc(num_blocks * AES_128_BLOCK_SIZE_BYTES, 1);
    for (int i = 0; i < num_blocks; i++)
    {
        buf_t enc_input = xor_buffers(plaintext + i * AES_128_BLOCK_SIZE_BYTES, prev, AES_128_BLOCK_SIZE_BYTES);
        buf_t ciphertext_block = encrypt_block_aes128(enc_input, key);
        memcpy(out + i * AES_128_BLOCK_SIZE_BYTES, ciphertext_block, AES_128_BLOCK_SIZE_BYTES);

        prev = ciphertext_block;
    }

    *ciphertext_len = num_blocks * AES_128_BLOCK_SIZE_BYTES;
    return out;
}

buf_t encrypt_aes128_ecb(buf_t unpadded_plaintext, size_t unpadded_plaintext_len, buf_t key, size_t *ciphertext_len)
{
    size_t plaintext_len;
    buf_t plaintext = pad_pkcs7(unpadded_plaintext, unpadded_plaintext_len, AES_128_BLOCK_SIZE_BYTES, &plaintext_len);
    int num_blocks = plaintext_len / AES_128_BLOCK_SIZE_BYTES;

    buf_t out = calloc(num_blocks * AES_128_BLOCK_SIZE_BYTES, 1);
    for (int i = 0; i < num_blocks; i++)
    {
        buf_t ciphertext_block = encrypt_block_aes128(plaintext + i * AES_128_BLOCK_SIZE_BYTES, key);
        memcpy(out + i * AES_128_BLOCK_SIZE_BYTES, ciphertext_block, AES_128_BLOCK_SIZE_BYTES);
    }

    *ciphertext_len = num_blocks * AES_128_BLOCK_SIZE_BYTES;
    return out;
}

bool try_detect_ecb(unsigned char *ciphertext, size_t ciphertext_len)
{
    const size_t CIPHER_SIZE = AES_128_BLOCK_SIZE_BYTES;
    int num_segments = ciphertext_len / CIPHER_SIZE;
    for (int i = 0; i < num_segments; i++)
    {
        for (int j = i + 1; j < num_segments; j++)
        {
            if (are_bytes_equal(ciphertext + i * CIPHER_SIZE, ciphertext + j * CIPHER_SIZE, CIPHER_SIZE))
            {
                return true;
            }
        }
    }

    return false;
}

bool try_detect_ecb2(unsigned char *ciphertext, size_t ciphertext_len, int *segment_i, int *segment_j)
{
    const size_t CIPHER_SIZE = AES_128_BLOCK_SIZE_BYTES;
    int num_segments = ciphertext_len / CIPHER_SIZE;
    for (int i = 0; i < num_segments; i++)
    {
        for (int j = i + 1; j < num_segments; j++)
        {
            if (are_bytes_equal(ciphertext + i * CIPHER_SIZE, ciphertext + j * CIPHER_SIZE, CIPHER_SIZE))
            {
                *segment_i = i;
                *segment_j = j;
                return true;
            }
        }
    }

    return false;
}

buf_t random_aes128_key()
{
    return random_bytes(AES_128_KEY_SIZE_BYTES);
}

buf_t zero_iv()
{
    buf_t b = (buf_t) "\x00";
    return repeat_buffer(b, 1, 16);
}