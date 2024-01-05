#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../lib/hex_to_base64.c"
#include "../lib/xor_buffers.c"

typedef uint8_t *buf_t;

int arg_max(int *arr, size_t len, int *value)
{
    if (len == 0)
    {
        return -1;
    }

    *value = arr[0];
    int max_idx = 0;
    for (size_t i = 1; i < len; i++)
    {
        if (arr[i] > *value)
        {
            max_idx = i;
            *value = arr[i];
        }
    }

    return max_idx;
}

bool is_alpha(char c)
{
    return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z';
}

int english_score(char *str, size_t str_len)
{
    int score = 0;
    for (int i = 0; i < str_len; i++)
    {
        if (is_alpha(str[i]))
        {
            score += 1;
        }
    }
    return score;
}

int main(int argc, char const *argv[])
{
    char x[] = "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736";
    const size_t x_len = sizeof(x) / sizeof(char);

    buf_t buf1, buf2;
    size_t buf1_len, buf2_len;
    buf1 = hex_to_bytes(x, x_len, &buf1_len);

    const int NUM_CANDIDATES = 256;
    char *candidate_bufs[NUM_CANDIDATES];
    int scores[NUM_CANDIDATES];
    for (int c = 0; c < NUM_CANDIDATES; c++)
    {
        buf2 = malloc(buf1_len);
        for (int i = 0; i < buf1_len; i++)
        {
            buf2[i] = c;
        }

        buf_t buf3 = xor_buffers(buf1, buf2, buf1_len);
        char *buf3_ascii = (char *)buf3;
        candidate_bufs[c] = buf3_ascii;
        scores[c] = english_score(buf3_ascii, buf1_len);
    }

    int max_score;
    int i = arg_max(scores, NUM_CANDIDATES, &max_score);
    printf("[%c] %s\n", i, candidate_bufs[i]);
}
