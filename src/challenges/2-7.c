#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../lib/common.c"
#include "../lib/crypto.c"

void validate(buf_t buf, size_t buf_len)
{
    buf_t out;
    size_t out_len;
    bool is_valid = try_unpad_pkcs7(buf, buf_len, AES_128_BLOCK_SIZE_BYTES, &out, &out_len);

    if (is_valid)
    {
        printf("Valid - %s (%zu)\n", out, out_len);
    }
    else
    {
        printf("Invalid\n");
    }
}

int main(int argc, char const *argv[])
{
    byte_t test1[] = "ICE ICE BABY\x04\x04\x04\x04";
    byte_t test2[] = "ICE ICE BABY\x05\x05\x05\x05";
    byte_t test3[] = "ICE ICE BABY\x01\x02\x03\x04";
    byte_t test4[] = "ICE ICE BABY";
    byte_t test5[] = "ICE ICE BABY\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10";

    validate(test1, 16);
    validate(test2, 16);
    validate(test3, 16);
    validate(test4, 12);
    validate(test5, 28);
}
