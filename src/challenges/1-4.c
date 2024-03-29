#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../lib/encoding.c"
#include "../lib/english_match.c"

int main(int argc, char const *argv[])
{
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char chr;

    fp = fopen("./1-4.txt", "r");
    if (fp == NULL)
    {
        return EXIT_FAILURE;
    }

    int line_idx = 0;
    char *candidates[1000];
    int scores[1000];
    while ((read = getline(&line, &len, fp)) != -1)
    {
        size_t buf1_len;
        buf_t buf1 = hex_to_bytes(line, read - 1, &buf1_len);

        int score;
        char *ans = get_best_english_match(buf1, buf1_len, &chr, &score);

        candidates[line_idx] = ans;
        scores[line_idx] = score;

        line_idx += 1;
    }

    int best_score = 1;
    int best_idx = arg_max(scores, line_idx, &best_score);

    printf("[%d] [%d] %s", best_idx, best_score, candidates[best_idx]);
    fclose(fp);
}
