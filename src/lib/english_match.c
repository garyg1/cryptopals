#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "./hex_to_base64.c"
#include "./common.c"

int english_score(char *str, size_t str_len)
{
    int score = 0;
    for (int i = 0; i < str_len; i++)
    {
        if (is_alpha(str[i]))
        {
            score += get_english_freq_score(str[i]);
        }
        else
        {
            score -= 15;
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

char *get_best_english_match(buf_t buf1, size_t buf1_len, char *out, int *max_score)
{
    const int NUM_CANDIDATES = 256;
    char *candidate_bufs[NUM_CANDIDATES];
    int scores[NUM_CANDIDATES];
    for (int c = 0; c < NUM_CANDIDATES; c++)
    {
        byte_t chr = (byte_t)c;
        buf_t buf3 = repeating_key_xor(buf1, buf1_len, &chr, 1);
        char *buf3_ascii = (char *)buf3;
        candidate_bufs[c] = buf3_ascii;
        scores[c] = english_score(buf3_ascii, buf1_len);
    }

    int i = arg_max(scores, NUM_CANDIDATES, max_score);

    *out = i;
    return candidate_bufs[i];
}