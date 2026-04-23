#include <stdio.h>
#include <string.h>
#include "attack.h"

void print_hex(const char *label, byte *x, size_t n)
{
    printf("%s = ", label);
    print_bytes(x, n);
    printf("\n");
}

int main()
{
    byte m[1 << 14];   // 16KB message
    byte m2[1 << 14];
    byte h1[HLEN], h2[HLEN];

    size_t len = sizeof(m);

    random_bytes(m, len);

    printf("===== ATTACK TEST =====\n");

    hash(m, len, h1);

    double cost = attack(m, len, m2);

    hash(m2, len, h2);

    print_hex("H(m)", h1, HLEN);
    print_hex("H(m2)", h2, HLEN);

    printf("log2(samples) ≈ %.2f\n", cost);

    if (memcmp(h1, h2, HLEN) == 0)
        printf("SUCCESS: collision found\n");
    else
        printf("FAIL\n");

    return 0;
}