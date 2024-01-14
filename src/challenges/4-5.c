#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../lib/common.c"
#include "../lib/crypto.c"
#include "../lib/clibs_sha1.c"

size_t discard;
byte_t key[] = "YELLOW SUBMARINE 123";
const size_t key_len = 20;

void hash_oracle(buf_t plaintext, size_t plaintext_len, buf_t hash_out)
{
    const size_t input_len = key_len + plaintext_len;
    byte_t input[input_len + 1];
    concat_buffers2_inplace(input, key, key_len, plaintext, plaintext_len);
    input[input_len] = '\0';

    clibs_SHA1(hash_out, input, input_len);
}

bool verify_oracle(buf_t plaintext, size_t plaintext_len, buf_t sha_digest)
{
    byte_t hash_out[20];
    hash_oracle(plaintext, plaintext_len, hash_out);

    return are_bytes_equal(hash_out, sha_digest, 20);
}

int main(int argc, char const *argv[])
{
    byte_t message[] = "comment1=cooking%20MCs;userdata=foo;comment2=%20like%20a%20pound%20of%20bacon";
    const size_t message_len = sizeof(message) - 1;

    byte_t hash_out_1[20];
    hash_oracle(message, message_len, hash_out_1);

    byte_t extension_msg[] = "&admin=true";
    size_t extension_msg_len = sizeof(extension_msg) - 1;

    byte_t synthetic_message[1000];
    size_t synthetic_message_len;
    byte_t md_hash[1000];
    size_t md_hash_len;

    for (int key_len_guess = 0; key_len_guess <= 100; key_len_guess++)
    {
        SHA1_CTX ctx;
        byte_t hash_out_2[20];
        byte_t hash_out_3[20];
        int count = 8 * (key_len_guess + message_len);
        int sha_init_val = count;
        if (sha_init_val % 512 != 0)
        {
            sha_init_val += (512 - (sha_init_val % 512));
        }
        SHA1InitFromDigest(&ctx, hash_out_1, sha_init_val, 0);
        for (int i = 0; i < extension_msg_len; i++)
        {
            SHA1Update(&ctx, (const unsigned char *)extension_msg + i, 1);
        }
        SHA1Final(hash_out_2, &ctx);

        char *guess = bytes_to_hex(hash_out_2, 20, &discard);

        md_hash[0] = 0200;
        md_hash_len = (sha_init_val - count) / 8;
        for (int i = 1; i < md_hash_len - 4; i++)
        {
            md_hash[i] = 0;
        }
        for (int i = 3; i >= 0; i--)
        {
            md_hash[md_hash_len - 1 - i] = (count >> (i * 8)) & 0xff;
        }

        concat_buffers3_inplace(synthetic_message,
                                message, message_len,
                                md_hash, md_hash_len,
                                extension_msg, extension_msg_len);
        synthetic_message_len = message_len + md_hash_len + extension_msg_len;
        hash_oracle(synthetic_message, synthetic_message_len, hash_out_3);

        if (are_bytes_equal(hash_out_2, hash_out_3, 20))
        {
            char *actual = bytes_to_hex(hash_out_3, 20, &discard);
            printf("key len: %d\n", key_len_guess);
            printf("guess: %s\n", guess);
            printf("actual: %s\n", actual);
            print_ascii_only(synthetic_message, synthetic_message_len);
            break;
        }
    }
}