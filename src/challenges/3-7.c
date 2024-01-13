#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include "../lib/mersenne.c"
#include "../lib/common.c"

void make_mt(struct mersenne_state *state)
{
    long seed = time(NULL);

    mt_seed(state, seed);
}

void copy_mt(struct mersenne_state *state, struct mersenne_state *state_copy)
{
    const long n = 624L;
    state_copy->index = 0;
    for (int i = 0; i < n; i++)
    {
        state_copy->mt[i] = untemper(mt_random(state));
    }
}

int main(int argc, char const *argv[])
{
    const int NUM_TESTS = 100000;
    for (long i = 0; i < NUM_TESTS; i++)
    {
        long i1 = randrange(0, 100000000);
        long i2 = temper(i1);
        long i3 = untemper(i2);

        if (i1 != i3)
        {
            printf("%ld != %ld, untemper is wrong\n", i1, i3);
            return 1;
        }
    }

    printf("untemper SUCCESS (tests=%d)\n", NUM_TESTS);

    struct mersenne_state state;
    make_mt(&state);

    struct mersenne_state state_copy_expected;
    memcpy(&state_copy_expected, &state, sizeof(struct mersenne_state));

    struct mersenne_state state_copy_actual;
    copy_mt(&state, &state_copy_actual);

    for (int i = 0; i < NUM_TESTS; i++)
    {
        long expected = mt_random(&state_copy_expected);
        long actual = mt_random(&state_copy_actual);

        if (expected != actual)
        {
            printf("%ld != %ld, copy_mt is wrong\n", expected, actual);
            return 1;
        }
    }

    printf("copy_mt SUCCESS (tests=%d)\n", NUM_TESTS);
}
