#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../lib/edit_distance.c"
#include "../lib/hex_to_base64.c"
#include "../lib/common.c"
#include "../lib/english_match.c"

char *read_base64_from_file(char *filename, size_t *base64_len)
{
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        exit(EXIT_FAILURE);
    }

    int line_idx = 0;
    char *lines[1000];
    size_t sizes[1000];
    size_t total = 0;
    while ((read = getline(&line, &len, fp)) != -1)
    {
        lines[line_idx] = strdup(line);
        if (line[read - 1] == '\n')
        {
            read -= 1;
        }
        sizes[line_idx] = read;
        total += read;
        line_idx += 1;
    }

    char *base64 = calloc(total + 1, sizeof(char));

    size_t offset = 0;
    for (int i = 0; i < line_idx; i++)
    {
        memcpy(base64 + offset, lines[i], sizes[i]);
        offset += sizes[i];
    }

    *base64_len = total;
    return base64;
}

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
    char *base64 = read_base64_from_file("./1-6.txt", &base64_len);

    size_t bytes_len;
    buf_t bytes = base64_to_bytes(base64, base64_len, &bytes_len);

    const int MIN_KEYSIZE = 2;
    const int MAX_KEYSIZE = 40;
    double keysizes[MAX_KEYSIZE - MIN_KEYSIZE];
    int keysizes_len = MAX_KEYSIZE - MIN_KEYSIZE;
    for (int keysize = MIN_KEYSIZE; keysize <= MAX_KEYSIZE; keysize++)
    {
        buf_t chunk1 = malloc(keysize);
        buf_t chunk2 = malloc(keysize);
        keysizes[keysize - MIN_KEYSIZE] = 0;
        for (int i = 0; i < 20; i++)
        {
            memcpy(chunk1, &bytes[keysize * i], keysize);
            memcpy(chunk2, &bytes[keysize * (i + 1)], keysize);
            int dst = edit_distance(chunk1, chunk2, keysize);
            keysizes[keysize - MIN_KEYSIZE] += dst * 0.1 / keysize;
        }
    }

    double min_keysize;
    int candidate_keysize = arg_min_d(keysizes, keysizes_len, &min_keysize) + MIN_KEYSIZE;
    printf("candidate keysize: %d (score: %f)\n", candidate_keysize, min_keysize);

    char *key = malloc(candidate_keysize);
    for (int i = 0; i < candidate_keysize; i++)
    {
        size_t block_len;
        buf_t block = every_nth_char(bytes, bytes_len, candidate_keysize, i, &block_len);
        char key_chr;
        int max_score;
        char *decoded = get_best_english_match(block, block_len, &key_chr, &max_score);
        key[i] = key_chr;
    }

    printf("key: %s\n", key);
    buf_t decrypted = repeating_key_xor(bytes, bytes_len, key, candidate_keysize);
    printf("plaintext: %s\n", decrypted);
}