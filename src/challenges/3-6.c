#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include "../lib/mersenne.c"
#include "../lib/common.c"

long sut()
{
    int interval1 = randrange(40, 100);
    sleep(interval1);

    struct mersenne_state state;
    int seed = time(NULL);
    printf("actual seed: %d\n", seed);
    mt_seed(&state, seed);

    int interval2 = randrange(40, 100);
    sleep(interval2);

    return mt_random(&state);
}

int find_seed(long r)
{
    static int NUM_TRIES = 100000;
    int seed_guess = time(NULL);
    for (int tries = 0; tries < NUM_TRIES; tries++)
    {
        struct mersenne_state state;
        mt_seed(&state, seed_guess);

        if (mt_random(&state) == r)
        {
            return seed_guess;
        }

        seed_guess--;
    }

    return -1;
}

int main(int argc, char const *argv[])
{
    long r = sut();
    long seed_guess = find_seed(r);

    printf("reconstructed seed: %zu\n", seed_guess);
}
