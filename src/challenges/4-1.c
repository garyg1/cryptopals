#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include "../lib/common.c"
#include "../lib/crypto.c"
#include "../lib/encoding.c"

buf_t get_file_contents(size_t *plaintext_len)
{
    byte_t key[] = "YELLOW SUBMARINE";

    size_t input_len;
    char *input = read_multiline_from_file("./4-1.txt", &input_len);

    size_t ciphertext_len;
    buf_t ciphertext = base64_to_bytes(input, input_len, &ciphertext_len);

    buf_t plaintext = decrypt_aes128_ecb(ciphertext, ciphertext_len, key, plaintext_len);

    free(input);
    free(ciphertext);

    return plaintext;
}

void edit(buf_t ciphertext, buf_t key, buf_t nonce, size_t offset_bytes, buf_t new_text, size_t new_text_len)
{
    size_t offset_blocks = offset_bytes / AES_128_BLOCK_SIZE_BYTES;
    size_t offset_in_block = offset_bytes % AES_128_BLOCK_SIZE_BYTES;

    size_t ciphertext_2_len = offset_in_block + new_text_len;
    byte_t ciphertext_2[ciphertext_2_len];

    size_t new_text_2_len = offset_in_block + new_text_len;
    byte_t new_text_2[new_text_2_len];
    memset(new_text_2, 0, offset_in_block);
    memcpy(new_text_2 + offset_in_block, new_text, new_text_len);

    encrypt_aes128_ctr_offset(new_text_2, new_text_2_len, key, nonce, ciphertext_2, offset_blocks);
    memcpy(ciphertext + offset_bytes, ciphertext_2 + offset_in_block, new_text_len);
}

int main(int argc, char const *argv[])
{
    size_t plaintext_len;
    buf_t plaintext = get_file_contents(&plaintext_len);

    byte_t key[128];
    byte_t nonce[128];
    random_bytes_inplace(key, 128);
    random_bytes_inplace(nonce, 128);

    byte_t ciphertext[plaintext_len];
    size_t ciphertext_len = plaintext_len;
    encrypt_aes128_ctr(plaintext, plaintext_len, key, nonce, ciphertext);

    byte_t plaintext2[plaintext_len];
    decrypt_aes128_ctr(ciphertext, ciphertext_len, key, nonce, plaintext2);

    byte_t replacement[] = "\x00\x00\x00\x00\x00\x00\x00\x00"
                           "\x00\x00\x00\x00\x00\x00\x00\x00";
    size_t replacement_len = 16;

    byte_t recovered_plaintext[plaintext_len + 1];
    recovered_plaintext[plaintext_len] = '\0';

    byte_t ciphertext_copy[ciphertext_len];
    for (int i = 0; i < ciphertext_len; i += AES_128_BLOCK_SIZE_BYTES)
    {
        memcpy(ciphertext_copy, ciphertext, ciphertext_len);
        edit(ciphertext_copy, key, nonce, i, replacement, replacement_len);
        xor_buffers_inplace(recovered_plaintext + i, ciphertext + i, ciphertext_copy + i, AES_128_BLOCK_SIZE_BYTES);
    }

    printf("%s\n", recovered_plaintext);
    free(plaintext);
}
