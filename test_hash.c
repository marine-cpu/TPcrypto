#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "hash.h"

void print_digests(byte *h, int count) {
    for (int i = 0; i < count; i++) {
        printf("h[%d] = ", i);
        print_bytes(h + i * HLEN, HLEN);
        printf("\n");
    }
}

int main() {

    byte h[HLEN];
    byte m[64];
    byte inter[10 * HLEN];

    printf("==========================\n");
    printf("Message size: %d (%d bytes)\n", BLOCKSIZE, BLOCKSIZE/8);
    printf("Key size: %d (%d bytes)\n", 2*BLOCKSIZE, (2*BLOCKSIZE)/8);
    printf("==========================\n");


    // COMPRESSION
    printf("*** COMPRESSION\n");

    if (BLOCKSIZE == 32)
        read_bytes("03020100", h, HLEN);
    else if (BLOCKSIZE == 48)
        read_bytes("050403020100", h, HLEN);
    else
        read_bytes("0706050403020100", h, HLEN);

    if (BLOCKSIZE == 32)
        read_bytes("0123456789abcdef", m, BLEN);
    else if (BLOCKSIZE == 48)
        read_bytes("0123456789abcdef01234567", m, BLEN);
    else
        read_bytes("0123456789abcdef0123456789abcdef", m, BLEN);

    printf("m = "); print_bytes(m, BLEN); printf("\n");
    printf("h0 = "); print_bytes(h, HLEN); printf("\n");

    compression(h, m);

    printf("f(h0,m) = "); print_bytes(h, HLEN); printf("\n");

    // HASH TEST 1
    printf("*** HASH\n");

    read_bytes("01", m, 1);

    hash(m, 1, h);

    printf("Message: m = 01\n");
    printf("Digest: h = "); print_bytes(h, HLEN); printf("\n");

    intermediate_digests(m, 1, inter);

    printf("Intermediate digests:\n");
    print_digests(inter, 3);

    // HASH TEST 2
    if (BLOCKSIZE == 32)
        read_bytes("0123456789abcdef", m, 8);
    else if (BLOCKSIZE == 48)
        read_bytes("0123456789abcdef01234567", m, 12);
    else
        read_bytes("0123456789abcdef0123456789abcdef", m, 16);

    hash(m, BLEN, h);

    printf("Message: m = ");
    print_bytes(m, BLEN);
    printf("\n");

    printf("Digest: h = ");
    print_bytes(h, HLEN);
    printf("\n");

    intermediate_digests(m, BLEN, inter);

    printf("Intermediate digests:\n");
    print_digests(inter, 3);

    return 0;
}