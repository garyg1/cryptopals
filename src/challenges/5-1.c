#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../lib/common.c"
#include "../lib/bigint.c"

int main(int argc, char const *argv[])
{
    printf("%u\n", UINT32_MAX + UINT32_MAX + 2);

    {
        printf("[0xffffffff, 0xff] + [0xe0, 0xee]\n");
        bigint b1;
        bigint_init(&b1);
        b1.data[1] = UINT32_MAX;
        b1.data[0] = 0xff;

        bigint_print(&b1);

        bigint b2;
        bigint_init(&b2);
        b2.data[1] = 0xe0;
        b2.data[0] = 0xee;

        bigint b3;
        bigint_init(&b3);
        bigint_add(&b3, &b1, &b2);

        bigint_print(&b3);
    }
    {
        printf("321 * 12\n");
        bigint b1;
        bigint_init(&b1);
        b1.data[2] = 3;
        b1.data[1] = 2;
        b1.data[0] = 1;

        bigint b2;
        bigint_init(&b2);
        b2.data[1] = 1;
        b2.data[0] = 2;

        bigint out;
        bigint_init(&out);
        bigint_multiply(&out, &b1, &b2);

        bigint_print(&b1);
        bigint_print(&b2);
        bigint_print(&out);

        printf("321 * 2\n");
        bigint_init(&out);
        bigint_multiply_scalar(&out, &b1, UINT32_MAX);
        bigint_print(&b1);
        bigint_print(&out);
    }
}
