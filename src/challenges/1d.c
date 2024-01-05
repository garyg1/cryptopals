#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../lib/hex_to_base64.c"
#include "../lib/xor_buffers.c"
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
        exit(EXIT_FAILURE);
    }

    int line_idx = 0;
    char *candidates[1000];
    int scores[1000];
    while ((read = getline(&line, &len, fp)) != -1)
    {
        int score;
        char *ans = get_best_english_match(line, read - 1, &chr, &score);
        candidates[line_idx] = ans;
        scores[line_idx] = score;

        line_idx += 1;
    }

    int best_score = 1;
    int best_idx = arg_max(scores, line_idx, &best_score);

    printf("[%d] [%d] %s", best_idx, best_score, candidates[best_idx]);
    fclose(fp);
}
