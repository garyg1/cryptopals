#pragma once
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

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

int arg_max_d(double *arr, size_t len, double *value)
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

int arg_min(int *arr, size_t len, int *value)
{
    if (len == 0)
    {
        return -1;
    }

    *value = arr[0];
    int min_idx = 0;
    for (size_t i = 1; i < len; i++)
    {
        if (arr[i] < *value)
        {
            min_idx = i;
            *value = arr[i];
        }
    }

    return min_idx;
}

int arg_min_d(double *arr, size_t len, double *value)
{
    if (len == 0)
    {
        return -1;
    }

    *value = arr[0];
    int min_idx = 0;
    for (size_t i = 1; i < len; i++)
    {
        if (arr[i] < *value)
        {
            min_idx = i;
            *value = arr[i];
        }
    }

    return min_idx;
}

bool is_alpha(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == ' ';
}

int get_english_freq_score(char c)
{
    if (c >= 'A' && c <= 'Z')
    {
        c = c - 'A' + 'a';
    }
    switch (c)
    {
    case 'e':
    case 'a':
    case 'i':
    case 'o':
    case 'u':
        return 30;
    case 'r':
    case 's':
    case 't':
    case 'l':
    case 'n':
    case ' ':
        return 10;
    case 'q':
    case 'z':
    case 'x':
        return 1;
    default:
        return 5;
    }
}


char *read_multiline_from_file(char *filename, size_t *base64_len)
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
