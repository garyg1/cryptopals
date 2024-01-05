#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "./hex_to_base64.c"
#include "./xor_buffers.c"
#include "./common.c"

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
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == ' ';
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
        else
        {
            score -= 3;
        }
    }
    return score;
}

buf_t repeating_key_xor(buf_t buf1, size_t buf1_len, buf_t chars, size_t chars_len)
{
    buf_t buf2 = malloc(buf1_len);
    int chars_i = 0;
    for (int i = 0; i < buf1_len; i++)
    {
        buf2[i] = chars[chars_i];
        chars_i = (chars_i + 1) % chars_len;
    }

    buf_t buf3 = xor_buffers(buf1, buf2, buf1_len);
    return buf3;
}

char *get_best_english_match(char *x, size_t x_len, char *out, int *max_score)
{
    buf_t buf1;
    size_t buf1_len;
    buf1 = hex_to_bytes(x, x_len, &buf1_len);

    const int NUM_CANDIDATES = 256;
    char *candidate_bufs[NUM_CANDIDATES];
    int scores[NUM_CANDIDATES];
    for (int c = 0; c < NUM_CANDIDATES; c++)
    {
        uint8_t chr = (uint8_t) c;
        buf_t buf3 = repeating_key_xor(buf1, buf1_len, &chr, 1);
        char *buf3_ascii = (char *)buf3;
        candidate_bufs[c] = buf3_ascii;
        scores[c] = english_score(buf3_ascii, buf1_len);
    }

    int i = arg_max(scores, NUM_CANDIDATES, max_score);

    *out = i;
    return candidate_bufs[i];
}