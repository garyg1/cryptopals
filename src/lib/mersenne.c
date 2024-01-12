#pragma onces

// https://en.wikipedia.org/wiki/Mersenne_Twister
const long a = 0x9908b0df;
const long b = 0x9d2c5680;
const long c = 0xefc60000;
const long d = 0xffffffff;
const long f = 0x6c078965;
const long l = 18;
const long lower_mask = 0x7fffffff;
const long m = 397;
const long n = 624;
const long r = 31;
const long s = 7;
const long t = 15;
const long u = 11;
const long upper_mask = 0x80000000;
const long w = 32;

struct mersenne_state
{
    long mt[n];
    int index;
};

void seed_mt(struct mersenne_state *state, long seed)
{
    state->mt[0] = seed & d;
    for (long i = 1; i < n; i++)
    {
        state->mt[i] = (f * (state->mt[i - 1] ^ (state->mt[i - 1] >> (w - 2))) + i) & d;
    }
    state->index = n;
}

void generate_block(struct mersenne_state *state)
{
    for (int i = 0; i < n; i++)
    {
        long x = (state->mt[i] & upper_mask) | (state->mt[(i + 1) % n] & lower_mask);
        long xA = x >> 1;
        if (x % 2 == 1)
        {
            xA ^= a;
        }
        state->mt[i] = state->mt[(i + m) % n] ^ xA;
    }
}

long mt_random(struct mersenne_state *state)
{
    if (state->index == n)
    {
        generate_block(state);
        state->index = 0;
    }

    long ans = state->mt[state->index];
    ans ^= (ans >> u) & d;
    ans ^= (ans << s) & b;
    ans ^= (ans << t) & c;
    ans ^= ans >> l;

    state->index++;
    return ans;
}