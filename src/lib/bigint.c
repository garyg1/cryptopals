#pragma once

#include <stdlib.h>
#include "./common.c"

const int BIGINT_MAXDATA = 1000;

typedef struct
{
    uint32_t data[BIGINT_MAXDATA]; // big-endian
} bigint;

void bigint_init(bigint *bi)
{
    memset(bi->data, 0, sizeof(bi->data));
}

void multiply32(uint32_t a, uint32_t b, uint32_t *out_hi, uint32_t *out_lo)
{
    uint64_t ab = ((uint64_t)a) * ((uint64_t)b);
    *out_hi = ab >> 32;
    *out_lo = ab & UINT32_MAX;
}

void bigint_shift(bigint *out, const bigint *bi, int num)
{
    for (int i = 0; i < BIGINT_MAXDATA; i++)
    {
        int source_idx = i - num;
        if (source_idx >= 0 && source_idx <= BIGINT_MAXDATA)
        {
            out[i] = bi[source_idx];
        }
    }
}

void bigint_add_leftshift(bigint *out, const bigint *bi1, uint32_t value_to_add, int leftshift)
{
    uint32_t carry = 0;
    for (int i = leftshift; i < BIGINT_MAXDATA; i++)
    {
        // ignore overflow
        uint32_t lhs = bi1->data[i];
        out->data[i] = lhs + value_to_add + carry;

        uint32_t diff = UINT32_MAX - (value_to_add + carry);
        value_to_add = 0;
        carry = 0;
        if (lhs > diff)
        {
            carry = 1;
        }
        else
        {
            break;
        }
    }
}

void bigint_add(bigint *out, const bigint *bi1, const bigint *bi2)
{
    uint32_t carry = 0;
    for (int i = 0; i < BIGINT_MAXDATA; i++)
    {
        // ignore overflow
        out->data[i] = bi1->data[i] + bi2->data[i] + carry;

        uint32_t diff = UINT32_MAX - (bi2->data[i] + carry);
        carry = 0;
        if (bi1->data[i] > diff)
        {
            carry = 1;
        }
    }
}

void bigint_multiply(bigint *out, const bigint *bi1, const bigint *bi2)
{
    for (int i = 0; i < BIGINT_MAXDATA; i++)
    {
        for (int j = 0; j < BIGINT_MAXDATA; j++)
        {
            uint32_t hi;
            uint32_t lo;
            multiply32(bi1->data[i], bi2->data[j], &hi, &lo);
            bigint_add_leftshift(out, out, lo, i + j);
            if (hi != 0)
            {
                bigint_add_leftshift(out, out, hi, i + j + 1);
            }
        }
    }
    /*
    abcd
  *  efg
    ====
    (a * g) << 3 + (b * g) << 2 + (c * g) << 1 + (d * g) << 0
    (a * f) << 4 + (b * f) << 3 + (c * f) << 2 + (d * f) << 1
  + (a * e) << 5 + (b * e) << 4 + (c * e) << 3 + (d * e) << 2
    ====
    */
}

void bigint_multiply_scalar(bigint *out, const bigint *bi, uint32_t scalar)
{
    for (int i = 0; i < BIGINT_MAXDATA; i++)
    {
        uint32_t hi;
        uint32_t lo;
        multiply32(bi->data[i], scalar, &hi, &lo);

        bigint_add_leftshift(out, out, lo, i);
        if (hi != 0)
        {
            bigint_add_leftshift(out, out, hi, i + 1);
        }
    }
}

void bigint_init_b10(bigint *bi, const char *b10)
{
    bigint_init(&bi);

    size_t b10_len = strlen(b10);
    for (int i = 0; i < b10_len; i++)
    {
        bigint digit;
        bigint_init(&digit);
        digit.data[0] = b10[i] - '0';
        for (int j = b10_len - 1; j > i; j--)
        {
            bigint_multiply_scalar(&digit, &digit, 10);
        }

        bigint_add(&bi, &bi, &digit);
    }
}

void bigint_print(const bigint *bi)
{
    bool found_nonzero = false;
    for (int i = BIGINT_MAXDATA - 1; i >= 0; i--)
    {
        if (found_nonzero || bi->data[i] != 0)
        {
            if (found_nonzero)
            {
                printf("%08x ", bi->data[i]);
            }
            else
            {
                printf("%x ", bi->data[i]);
                found_nonzero = true;
            }
        }
    }

    printf("\n");
}