#pragma once
#include <stdlib.h>
#include <stdbool.h>

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