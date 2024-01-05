#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../lib/edit_distance.c"
#include "../lib/hex_to_base64.c"
#include "../lib/common.c"

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

int main(int argc, char const *argv[])
{
    unsigned char s1[] = "this is a test";
    unsigned char s2[] = "wokka wokka!!!";
    printf("%d\n", edit_distance(s1, s2, sizeof(s1) - 1));

    size_t base64_len;
    char *base64 = read_base64_from_file("./1-6.txt", &base64_len);

    size_t bytes_len;
    buf_t bytes = base64_to_bytes(base64, base64_len, &bytes_len);

    printf("%zu %s\n", base64_len, base64);
}
