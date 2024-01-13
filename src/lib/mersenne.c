#pragma once

#include <stdbool.h>
#include "./common.c"

// https://en.wikipedia.org/wiki/Mersenne_Twister
static const long a = 0x9908b0df;
static const long b = 0x9d2c5680;
static const long c = 0xefc60000;
static const long d = 0xffffffff;
static const long f = 0x6c078965;
static const long l = 18;
static const long lower_mask = 0x7fffffff;
static const long m = 397;
static const long n = 624;
static const long r = 31;
static const long s = 7;
static const long t = 15;
static const long u = 11;
static const long upper_mask = 0x80000000;
static const long w = 32;

struct mersenne_state
{
    long mt[n];
    int index;
};

static void generate_block(struct mersenne_state *state)
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

long undo_xor_rs_masked(long value, int rightshift, long mask)
{
    // abcde ^ (0abcd & 01010) = a(a^b)c(c^d)e
    // a(a^b)c(c^d)e
    // abc(c^d)e
    for (int i = rightshift; i < 64; i++)
    {
        if (get_bit_l(mask, i))
        {
            bool actual = get_bit_l(value, i - rightshift) ^ get_bit_l(value, i);
            set_bit_l(&value, i, actual);
        }
    }

    return value;
}

long undo_xor_ls_masked(long value, int leftshift, long mask)
{
    // abcde ^ (bcde0 & 01011) = a(b^c)c(d^e)e
    // a(b^c)c(d^e)e
    // a(b^c)c(d^e)e
    for (int i = 63 - leftshift; i >= 0; i--)
    {
        if (get_bit_l(mask, i))
        {
            bool actual = get_bit_l(value, i) ^ get_bit_l(value, i + leftshift);
            set_bit_l(&value, i, actual);
        }
    }

    return value;
}

long temper(long value)
{
    value ^= (value >> u) & d;
    value ^= (value << s) & b;
    value ^= (value << t) & c;
    value ^= value >> l;

    return value;
}

long untemper(long value)
{
    value = undo_xor_rs_masked(value, l, 0xffffffffffffffff);
    value = undo_xor_ls_masked(value, t, c);
    value = undo_xor_ls_masked(value, s, b);
    value = undo_xor_rs_masked(value, u, d);

    return value;
}

void mt_seed(struct mersenne_state *state, long seed)
{
    state->mt[0] = seed & d;
    for (long i = 1; i < n; i++)
    {
        state->mt[i] = (f * (state->mt[i - 1] ^ (state->mt[i - 1] >> (w - 2))) + i) & d;
    }
    state->index = n;
}

long mt_random(struct mersenne_state *state)
{
    if (state->index == n)
    {
        generate_block(state);
        state->index = 0;
    }

    long ans = state->mt[state->index];
    ++state->index;

    return temper(ans);
}