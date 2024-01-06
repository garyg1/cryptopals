#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../lib/common.c"

int main(int argc, char const *argv[])
{
    size_t test_len = 26;
    buf_t test = (buf_t) "foo=bar&baz=qux&zap=zazzle";

    struct string_dict d;
    try_parse_kv(test, test_len, &d);

    print_string_dict(&d);
}
