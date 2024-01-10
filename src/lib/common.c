#pragma once
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
typedef unsigned char byte_t;
typedef byte_t *buf_t;

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
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == ' ' || c == '\'';
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

    char *base64 = (char *)calloc(total + 1, sizeof(char));

    size_t offset = 0;
    for (int i = 0; i < line_idx; i++)
    {
        memcpy(base64 + offset, lines[i], sizes[i]);
        offset += sizes[i];
    }

    *base64_len = total;
    return base64;
}

char **read_lines_from_file(char *filename, size_t *num_lines)
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
    while ((read = getline(&line, &len, fp)) != -1)
    {
        lines[line_idx] = strdup(line);
        line_idx += 1;
    }

    char **output = (char **)calloc(line_idx, sizeof(char *));
    for (int i = 0; i < line_idx; i++)
    {
        output[i] = strdup(lines[i]);
    }

    *num_lines = line_idx;
    return output;
}

bool are_bytes_equal(buf_t buf1, buf_t buf2, size_t size)
{
    for (int i = 0; i < size; i++)
    {
        if (buf1[i] != buf2[i])
        {
            return false;
        }
    }
    return true;
}

void fill_char(char *str, char c, int n)
{
    for (int i = 0; i < n; i++)
    {
        str[i] = c;
    }
}

void fill_byte(buf_t str, byte_t c, int n)
{
    for (int i = 0; i < n; i++)
    {
        str[i] = c;
    }
}

buf_t xor_buffers(buf_t b1, buf_t b2, size_t len)
{
    buf_t result = malloc(len);
    for (int i = 0; i < len; i++)
    {
        result[i] = b1[i] ^ b2[i];
    }

    return result;
}

buf_t xor_buffer_inplace(buf_t result, buf_t b1, buf_t b2, size_t len)
{
    for (int i = 0; i < len; i++)
    {
        result[i] = b1[i] ^ b2[i];
    }

    return result;
}

void random_bytes_inplace(buf_t result, size_t n)
{
    size_t len = n / 4;
    for (int i = 0; i < len; i++)
    {
        int r = rand();
        result[4 * i] = r & 0xff;
        result[4 * i + 1] = (r >> 8) & 0xff;
        result[4 * i + 2] = (r >> 16) & 0xff;
        result[4 * i + 3] = (r >> 24) & 0xff;
    }

    if (n % 4 > 0)
    {
        int r = rand();
        switch (n % 4)
        {
        case 3:
            result[4 * len + 2] = (r >> 16) & 0xff;
        case 2:
            result[4 * len + 1] = (r >> 8) & 0xff;
        case 1:
            result[4 * len] = r & 0xff;
            break;
        }
    }
}

buf_t random_bytes(size_t n)
{
    buf_t result = malloc(n);
    random_bytes_inplace(result, n);

    return result;
}

int randrange(int lo, int hi)
{
    int r = rand();
    return (r % (hi - lo)) + lo;
}

bool rand_bool()
{
    return randrange(0, 2) == 1;
}

buf_t repeat_buffer(const buf_t b1, size_t b1_len, int times)
{
    buf_t out = malloc(b1_len * times);
    for (int i = 0; i < times; i++)
    {
        memcpy(out + (i * b1_len), b1, b1_len);
    }
    return out;
}

buf_t buffer_substring(const buf_t b1, size_t len)
{
    buf_t out = malloc(len);
    memcpy(out, b1, len);
    return out;
}

buf_t copy_buffer(const buf_t b1, size_t len)
{
    return buffer_substring(b1, len);
}

buf_t concat_buffers2(const buf_t b1, size_t b1_len, const buf_t b2, size_t b2_len)
{
    buf_t out = malloc(b1_len + b2_len);
    memcpy(out, b1, b1_len);
    memcpy(out + b1_len, b2, b2_len);
    return out;
}

buf_t concat_buffers3(const buf_t b1, size_t b1_len, const buf_t b2, size_t b2_len, const buf_t b3, size_t b3_len)
{
    buf_t out = malloc(b1_len + b2_len + b3_len);
    memcpy(out, b1, b1_len);
    memcpy(out + b1_len, b2, b2_len);
    memcpy(out + b1_len + b2_len, b3, b3_len);
    return out;
}

buf_t concat_buffers4(const buf_t b1, size_t b1_len, const buf_t b2, size_t b2_len, const buf_t b3, size_t b3_len, const buf_t b4, size_t b4_len)
{
    buf_t out = malloc(b1_len + b2_len + b3_len + b4_len);
    memcpy(out, b1, b1_len);
    memcpy(out + b1_len, b2, b2_len);
    memcpy(out + b1_len + b2_len, b3, b3_len);
    memcpy(out + b1_len + b2_len + b3_len, b4, b4_len);
    return out;
}

bool get_bit(buf_t buf, int bit_idx)
{
    int offset = bit_idx / 8;
    byte_t mask = 0b10000000 >> (bit_idx % 8);
    return (buf[offset] & mask) != 0;
}

void set_bit(buf_t buf, int bit_idx, bool bit)
{
    byte_t mask = 0b10000000 >> (bit_idx % 8);
    int offset = bit_idx / 8;

    if (!bit)
    {
        mask = ~mask;
        *(buf + offset) = *(buf + offset) & mask;
    }
    else
    {
        buf[offset] |= mask;
    }
}

void increment_buffer_be(buf_t buf, size_t buf_len)
{
    bool inc = true;
    int i = buf_len - 1;
    while (inc && i >= 0)
    {
        if (buf[i] == 0xff)
        {
            buf[i] = 0x00;
            i -= 1;
        }
        else
        {
            buf[i] += 1;
            inc = false;
        }
    }
}

void increment_buffer_le(buf_t buf, size_t buf_len)
{
    bool inc = true;
    int i = 0;
    while (inc && i < buf_len)
    {
        if (buf[i] == 0xff)
        {
            buf[i] = 0x00;
            i += 1;
        }
        else
        {
            buf[i] += 1;
            inc = false;
        }
    }
}

int max(int a, int b)
{
    return (a > b) ? a : b;
}

int min(int a, int b)
{
    return (a < b) ? a : b;
}

struct string_dict
{
    size_t len;
    buf_t *keys;
    size_t *key_lens;
    buf_t *values;
    size_t *value_lens;
};

bool try_parse_kv(buf_t kv, size_t kv_len, struct string_dict *out)
{
    const byte_t EQ = '=';
    const byte_t SEP = '&';

    size_t kv_count = 1;
    for (int i = 0; i < kv_len; i++)
    {
        if (kv[i] == SEP)
        {
            kv_count += 1;
        }
    }

    out->keys = calloc(kv_count, sizeof(buf_t *));
    out->values = calloc(kv_count, sizeof(buf_t *));
    out->key_lens = calloc(kv_count, sizeof(buf_t *));
    out->value_lens = calloc(kv_count, sizeof(buf_t *));
    out->len = kv_count;

    byte_t curr_token[1000] = {0};
    int curr_token_i = 0;
    int curr_kv_i = 0;

    for (int i = 0; i < kv_len; i++)
    {
        char c = kv[i];
        if (c == EQ)
        {
            out->keys[curr_kv_i] = malloc(curr_token_i + 1);
            memcpy(out->keys[curr_kv_i], curr_token, curr_token_i);
            out->keys[curr_kv_i][curr_token_i] = '\0';
            out->key_lens[curr_kv_i] = curr_token_i;
            curr_token_i = 0;
        }
        else if (c == SEP)
        {
            out->values[curr_kv_i] = malloc(curr_token_i + 1);
            memcpy(out->values[curr_kv_i], curr_token, curr_token_i);
            out->values[curr_kv_i][curr_token_i] = '\0';
            out->value_lens[curr_kv_i] = curr_token_i;
            curr_token_i = 0;
            curr_kv_i += 1;
        }
        else
        {
            curr_token[curr_token_i++] = c;
        }
    }

    // last kv
    out->values[curr_kv_i] = malloc(curr_token_i);
    memcpy(out->values[curr_kv_i], curr_token, curr_token_i);
    out->value_lens[curr_kv_i] = curr_token_i;

    return true;
}

void print_string_dict(struct string_dict *dict)
{
    printf("(%zu)\n{\n", dict->len);
    for (int i = 0; i < dict->len; i++)
    {
        printf("\t%s (%zu): '%s' (%zu),\n", dict->keys[i], dict->key_lens[i], dict->values[i], dict->value_lens[i]);
    }
    printf("}\n");
}