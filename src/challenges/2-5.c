#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../lib/common.c"
#include "../lib/crypto.c"

buf_t profile_for(buf_t username, size_t username_len, size_t *out_len)
{
    const buf_t s = (buf_t) "uid=10&role=user&email=";
    *out_len = username_len + 23;
    return concat_buffers2(s, 23, username, username_len);
}

int main(int argc, char const *argv[])
{
    buf_t key = random_aes128_key();
    buf_t iv = zero_iv();

    size_t test_len = 26;
    buf_t test = (buf_t) "foo=bar&baz=qux&zap=zazzle";

    struct string_dict test_dict;
    try_parse_kv(test, test_len, &test_dict);

    print_string_dict(&test_dict);

    size_t encoded_profile_len;
    buf_t encoded_profile = profile_for((buf_t) "foor@bar.com", 12, &encoded_profile_len);

    size_t encrypted_profile_len;
    buf_t encrypted_profile = encrypt_aes128_cbc(encoded_profile, encoded_profile_len, key, iv, &encrypted_profile_len);

    size_t decrypted_profile_len;
    buf_t decrypted_profile = decrypt_aes128_cbc_unpad(encrypted_profile, encrypted_profile_len, key, iv, &decrypted_profile_len);

    struct string_dict decrypted_profile_dict;
    try_parse_kv(decrypted_profile, decrypted_profile_len, &decrypted_profile_dict);
    print_string_dict(&decrypted_profile_dict);
}
