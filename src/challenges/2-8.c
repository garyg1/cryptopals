#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../lib/common.c"
#include "../lib/crypto.c"

static buf_t key;
static buf_t iv;

void init_encryption_oracle()
{
    key = random_aes128_key();
    iv = random_aes128_iv();
}

buf_t encryption_oracle(buf_t input, size_t input_len, size_t *enc_len)
{
    const buf_t prefix = (buf_t) "comment1=cooking%20MCs&userdata=";
    const size_t prefix_len = 32;
    const buf_t suffix = (buf_t) "&comment2=%20like%20a%20pound%20of%20bacon";
    const size_t suffix_len = 42;

    size_t plaintext_len = prefix_len + input_len + suffix_len;
    buf_t plaintext = concat_buffers3(
        prefix, prefix_len,
        input, input_len,
        suffix, suffix_len);

    return encrypt_aes128_cbc(plaintext, plaintext_len, key, iv, enc_len);
}

buf_t decryption_oracle(buf_t ciphertext, size_t ciphertext_len, size_t *dec_len)
{
    return decrypt_aes128_cbc(ciphertext, ciphertext_len, key, iv, dec_len);
}

void solve()
{
    size_t input_len = 16;
    buf_t input = repeat_buffer((buf_t) "\xff", 1, 16);

    size_t ciphertext_len;
    buf_t ciphertext = encryption_oracle(input, input_len, &ciphertext_len);

    buf_t replacement = xor_buffers(
        (buf_t) "&comment2=%20lik",
        (buf_t) "&admin=true&com=",
        16);
    buf_t new_userdata = xor_buffers(replacement, ciphertext + 32, 16);
    memcpy(ciphertext + 32, new_userdata, 16);

    size_t plaintext2_len;
    buf_t plaintext2 = decryption_oracle(ciphertext, ciphertext_len, &plaintext2_len);

    struct string_dict sd;
    if (!try_parse_kv(plaintext2, plaintext2_len, &sd))
    {
        printf("Could not parse: %s\n", plaintext2);
        return;
    }

    print_string_dict(&sd);
}

int main(int argc, char const *argv[])
{
    init_encryption_oracle();
    solve();
}
