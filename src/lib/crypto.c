#pragma once
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <openssl/aes.h>
#include <openssl/ssl.h>
#include "./common.c"
#include "./encoding.c"

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

bool try_unpad_pkcs7(buf_t buf, size_t buf_len, size_t block_size, buf_t *unpadded, size_t *unpadded_len)
{
    size_t num_chars = (size_t)buf[buf_len - 1];
    if (num_chars > buf_len)
    {
        return false;
    }

    if (num_chars == 0)
    {
        return false;
    }

    byte_t expected_value = (byte_t)num_chars;
    for (int i = 0; i < num_chars; i++)
    {
        size_t buf_idx = buf_len - i - 1;
        if (buf[buf_idx] != expected_value)
        {
            return false;
        }
    }
    *unpadded_len = buf_len - num_chars;
    *unpadded = buffer_substring(buf, buf_len - num_chars + 1);
    (*unpadded)[buf_len - num_chars] = '\0';

    return true;
}

const size_t AES_128_BLOCK_SIZE_BYTES = 16;
const size_t AES_128_KEY_SIZE_BYTES = 16;
const size_t AES_128_CTR_NONCE_SIZE_BYTES = 8;
const size_t AES_128_CTR_BLOCKCOUNT_SIZE_BYTES = 8;

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

buf_t encrypt_block_aes128_inplace(buf_t buf, buf_t key, buf_t out)
{
    AES_KEY enc_key;
    AES_set_encrypt_key(key, AES_128_KEY_SIZE_BYTES * 8, &enc_key);

    AES_encrypt(buf, out, &enc_key);

    return out;
}

buf_t decrypt_block_aes128_inplace(buf_t buf, buf_t key, buf_t out)
{
    AES_KEY enc_key;
    AES_set_decrypt_key(key, AES_128_KEY_SIZE_BYTES * 8, &enc_key);

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

    byte_t ciphertext_block[AES_128_BLOCK_SIZE_BYTES];
    for (int i = 0; i < num_blocks; i++)
    {
        encrypt_block_aes128_inplace(plaintext + i * AES_128_BLOCK_SIZE_BYTES, key, ciphertext_block);
        memcpy(out + i * AES_128_BLOCK_SIZE_BYTES, ciphertext_block, AES_128_BLOCK_SIZE_BYTES);
    }

    *ciphertext_len = num_blocks * AES_128_BLOCK_SIZE_BYTES;
    return out;
}

buf_t decrypt_aes128_ecb(buf_t ciphertext, size_t ciphertext_len, buf_t key, size_t *plaintext_len)
{
    int num_blocks = ciphertext_len / AES_128_BLOCK_SIZE_BYTES;

    buf_t plaintext = calloc(num_blocks * AES_128_BLOCK_SIZE_BYTES + 1, 1);
    plaintext[num_blocks * AES_128_BLOCK_SIZE_BYTES] = '\0';

    byte_t plaintext_block[AES_128_BLOCK_SIZE_BYTES];
    for (int i = 0; i < num_blocks; i++)
    {
        decrypt_block_aes128_inplace(ciphertext + i * AES_128_BLOCK_SIZE_BYTES, key, plaintext_block);
        memcpy(plaintext + i * AES_128_BLOCK_SIZE_BYTES, plaintext_block, AES_128_BLOCK_SIZE_BYTES);
    }

    *plaintext_len = num_blocks * AES_128_BLOCK_SIZE_BYTES;
    return plaintext;
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

void encrypt_aes128_ctr_offset(buf_t plaintext, size_t plaintext_len, buf_t key, buf_t nonce, buf_t ciphertext, size_t offset_blocks)
{
    int num_blocks = (plaintext_len + AES_128_BLOCK_SIZE_BYTES - 1) / AES_128_BLOCK_SIZE_BYTES;

    byte_t encrypted_ctr[AES_128_BLOCK_SIZE_BYTES];
    byte_t ctr[AES_128_BLOCK_SIZE_BYTES];
    memcpy(ctr, nonce, AES_128_BLOCK_SIZE_BYTES);

    for (int i = 0; i < offset_blocks; i++)
    {
        increment_buffer_le(ctr + AES_128_CTR_NONCE_SIZE_BYTES, AES_128_CTR_BLOCKCOUNT_SIZE_BYTES);
    }

    for (int i = 0; i < num_blocks; i++)
    {
        encrypt_block_aes128_inplace(ctr, key, encrypted_ctr);
        size_t offset = i * AES_128_BLOCK_SIZE_BYTES;
        size_t curr_block_len = min(AES_128_BLOCK_SIZE_BYTES, plaintext_len - offset);
        xor_buffers_inplace(ciphertext + offset, plaintext + offset, encrypted_ctr, curr_block_len);

        increment_buffer_le(ctr + AES_128_CTR_NONCE_SIZE_BYTES, AES_128_CTR_BLOCKCOUNT_SIZE_BYTES);
    }
}

void encrypt_aes128_ctr(buf_t plaintext, size_t plaintext_len, buf_t key, buf_t nonce, buf_t ciphertext)
{
    encrypt_aes128_ctr_offset(plaintext, plaintext_len, key, nonce, ciphertext, 0);
}

void decrypt_aes128_ctr(buf_t ciphertext, size_t ciphertext_len, buf_t key, buf_t nonce, buf_t plaintext)
{
    int num_blocks = (ciphertext_len + AES_128_BLOCK_SIZE_BYTES - 1) / AES_128_BLOCK_SIZE_BYTES;

    byte_t encrypted_ctr[AES_128_BLOCK_SIZE_BYTES];
    byte_t ctr[AES_128_BLOCK_SIZE_BYTES];
    memcpy(ctr, nonce, AES_128_BLOCK_SIZE_BYTES);

    for (int i = 0; i < num_blocks; i++)
    {
        encrypt_block_aes128_inplace(ctr, key, encrypted_ctr);
        size_t offset = i * AES_128_BLOCK_SIZE_BYTES;
        size_t curr_block_len = min(AES_128_BLOCK_SIZE_BYTES, ciphertext_len - offset);
        xor_buffers_inplace(plaintext + offset, ciphertext + offset, encrypted_ctr, curr_block_len);

        increment_buffer_le(ctr + AES_128_CTR_NONCE_SIZE_BYTES, AES_128_CTR_BLOCKCOUNT_SIZE_BYTES);
    }
}

buf_t random_aes128_key()
{
    return random_bytes(AES_128_KEY_SIZE_BYTES);
}

void random_aes128_key_inplace(buf_t buf)
{
    random_bytes_inplace(buf, AES_128_BLOCK_SIZE_BYTES);
}

buf_t random_aes128_iv()
{
    return random_bytes(AES_128_BLOCK_SIZE_BYTES);
}

#define ZERO_BUFFER_16 "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"

buf_t zero_iv()
{
    buf_t b = (buf_t) "\x00";
    return repeat_buffer(b, 1, 16);
}
