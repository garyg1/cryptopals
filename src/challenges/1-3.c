#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../lib/hex_to_base64.c"
#include "../lib/english_match.c"

int main(int argc, char const *argv[])
{
    char x[] = "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736";
    const size_t x_len = sizeof(x) / sizeof(char);
    char chr;
    int score;
    buf_t buf1;
    size_t buf1_len;
    buf1 = hex_to_bytes(x, x_len, &buf1_len);
    char *ans = get_best_english_match(buf1, buf1_len - 1, &chr, &score);
    printf("[%c] %s [%d]\n", chr, ans, score);
}
