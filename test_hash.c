#include <stdio.h>
#include "utils.h"
#include "hash.h"

int main() {
    byte h[HLEN];
    byte m[BLEN];

    // IV
    if (BLOCKSIZE == 32)
        read_bytes("03020100", h, HLEN);
    else if (BLOCKSIZE == 48)
        read_bytes("050403020100", h, HLEN);
    else
        read_bytes("0706050403020100", h, HLEN);

    // message bloc
    read_bytes("0123456789abcdef", m, BLEN);

    printf("h0 = ");
    print_bytes(h, HLEN);
    printf("\n");

    printf("m  = ");
    print_bytes(m, BLEN);
    printf("\n");

    compression(h, m);

    printf("f(h0,m) = ");
    print_bytes(h, HLEN);
    printf("\n");

    return 0;
}