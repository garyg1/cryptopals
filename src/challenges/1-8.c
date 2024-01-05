#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../lib/common.c"
#include "../lib/hex_to_base64.c"

bool try_detect_ecb(unsigned char *ciphertext, size_t ciphertext_len)
{
    const int CIPHER_SIZE = 128 / 8;
    int num_segments = ciphertext_len / CIPHER_SIZE;
    for (int i = 0; i < num_segments; i++)
    {
        for (int j = i + 1; j < num_segments; j++)
        {
            if (are_bytes_equal(ciphertext + i * CIPHER_SIZE, ciphertext + j * CIPHER_SIZE, CIPHER_SIZE))
            {
                return true;
            }
        }
    }

    return false;
}

int main(int argc, char const *argv[])
{
    size_t num_lines;
    char **hex_lines = read_lines_from_file("./1-8.txt", &num_lines);
    for (size_t i = 0; i < num_lines; i++)
    {
        size_t bytes_len;
        buf_t bytes = hex_to_bytes(hex_lines[i], strlen(hex_lines[i]), &bytes_len);
        bool is_ecb = try_detect_ecb(bytes, bytes_len);
        if (is_ecb)
        {
            printf("%zu - %s\n", i, hex_lines[i]);
        }
    }
}
