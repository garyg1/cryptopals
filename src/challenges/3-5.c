#include <stdlib.h>
#include <stdio.h>
#include "../lib/mersenne.c"

int main(int argc, char const *argv[])
{
    struct mersenne_state mt_state;
    seed_mt(&mt_state, 0ul);

    for (int i = 0; i < 100; i++)
    {
        long r = mt_random(&mt_state);
        printf("%zu\n", r);
    }
}
