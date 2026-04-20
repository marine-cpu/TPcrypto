#include <stdio.h>
#include "utils.h"

#include "utils.h"
#include "hash.h"

void compression(byte h[HLEN], const byte m[BLEN]) {
    byte c[HLEN];

    //E_m(h)
    speck_enc(m, h, c);

    //h = E_m(h) xor h
    for (int i = 0; i < HLEN; i++) {
        h[i] ^= c[i];
    }
}

void hash(const byte *m, size_t len, byte h[HLEN]){
    
}

void intermediate_digests(const byte *m, size_t len, byte *h){

}