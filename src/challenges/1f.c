#include <stdlib.h>
#include <stdio.h>
#include "../lib/edit_distance.c"

int main(int argc, char const *argv[])
{
    unsigned char s1[] = "this is a test";
    unsigned char s2[] = "wokka wokka!!!";
    printf("%d\n", edit_distance(s1, s2, sizeof(s1) - 1));
}
