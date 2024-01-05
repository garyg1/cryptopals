#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../lib/edit_distance.c"
#include "../lib/hex_to_base64.c"
#include "../lib/common.c"
#include "../lib/english_match.c"

buf_t every_nth_char(buf_t arr, size_t arr_len, int n, int offset, size_t *_out_len)
{
    int out_len = (arr_len - offset) / n;
    buf_t out = malloc(out_len);
    for (int i = 0; i < out_len; i++)
    {
        int arr_idx = i * n + offset;
        out[i] = arr[arr_idx];
    }

    *_out_len = out_len;
    return out;
}

int main(int argc, char const *argv[])
{
    unsigned char s1[] = "this is a test";
    unsigned char s2[] = "wokka wokka!!!";
    printf("%d\n", edit_distance(s1, s2, sizeof(s1) - 1));

    size_t base64_len;
    char *base64 = read_multiline_from_file("./1-6.txt", &base64_len);

    size_t bytes_len;
    buf_t bytes = base64_to_bytes(base64, base64_len, &bytes_len);

    const int MIN_KEYSIZE = 2;
    const int MAX_KEYSIZE = 40;
    const int NUM_SAMPLE_CHUNKS = 20;
    double keysizes[MAX_KEYSIZE - MIN_KEYSIZE];
    int keysizes_len = MAX_KEYSIZE - MIN_KEYSIZE;
    buf_t chunk1 = malloc(MAX_KEYSIZE);
    buf_t chunk2 = malloc(MAX_KEYSIZE);
    for (int keysize = MIN_KEYSIZE; keysize <= MAX_KEYSIZE; keysize++)
    {
        keysizes[keysize - MIN_KEYSIZE] = 0;
        for (int i = 1; i < NUM_SAMPLE_CHUNKS; i++)
        {
            memcpy(chunk1, bytes, MAX_KEYSIZE);
            memcpy(chunk2, bytes + keysize * i, MAX_KEYSIZE);
            int dst = edit_distance(chunk1, chunk2, keysize);
            keysizes[keysize - MIN_KEYSIZE] += dst * 0.1 / keysize;
        }
    }

    double min_keysize = 0.0;
    int candidate_keysize = arg_min_d(keysizes, keysizes_len, &min_keysize) + MIN_KEYSIZE;
    printf("candidate keysize: %d, (score: %f)\n", candidate_keysize, min_keysize);

    byte_t *key = malloc(candidate_keysize + 1);
    for (int i = 0; i < candidate_keysize; i++)
    {
        size_t block_len;
        buf_t block = every_nth_char(bytes, bytes_len, candidate_keysize, i, &block_len);
        char key_chr;
        int max_score;
        get_best_english_match(block, block_len, &key_chr, &max_score);
        key[i] = (byte_t)key_chr;
    }

    printf("key: %s\n", key);
    buf_t decrypted = repeating_key_xor(bytes, bytes_len, key, candidate_keysize);
    printf("plaintext: %s\n", decrypted);
}