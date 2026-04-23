#include <string.h>
#include <stdint.h>
#include "utils.h"
#include "hash.h"


// COMPRESSION (Davies Meyer)
void compression(byte h[HLEN], const byte m[BLEN]) {

    byte tmp[HLEN];

    // m=key, h=state
    speck_enc(m, h, tmp);

    for (int i = 0; i < HLEN; i++) {
        h[i] ^= tmp[i];
    }
}

// HASH (Merkle Damgard simplifie)

void hash(const byte *m, size_t len, byte h[HLEN]) {

    byte block[BLEN];

    // IV
    if (BLOCKSIZE == 32)
        read_bytes("03020100", h, HLEN);
    else if (BLOCKSIZE == 48)
        read_bytes("050403020100", h, HLEN);
    else
        read_bytes("0706050403020100", h, HLEN);

    size_t i = 0;

    // blocs complets
    while (i + BLEN <= len) {
        memcpy(block, m + i, BLEN);
        compression(h, block);
        i += BLEN;
    }

    // dernier bloc (padding zero uniquement)
    size_t rem = len - i;

    if (rem > 0) {
        memset(block, 0, BLEN);
        memcpy(block, m + i, rem);
        compression(h, block);
    }
    // bloc longueur
    memset(block, 0, BLEN);

    uint64_t bitlen = (uint64_t)len * 8;

    memcpy(block, &bitlen, sizeof(uint64_t));

    compression(h, block);
}

// INTERMEDIATE DIGESTS
void intermediate_digests(const byte *m, size_t len, byte *out) {

    byte h[HLEN];
    byte block[BLEN];

    // IV
    if (BLOCKSIZE == 32)
        read_bytes("03020100", h, HLEN);
    else if (BLOCKSIZE == 48)
        read_bytes("050403020100", h, HLEN);
    else
        read_bytes("0706050403020100", h, HLEN);

    size_t i = 0;
    int idx = 0;

    // h0
    memcpy(out + idx * HLEN, h, HLEN);
    idx++;

    // blocs complets
    while (i + BLEN <= len) {
        memcpy(block, m + i, BLEN);
        compression(h, block);

        memcpy(out + idx * HLEN, h, HLEN);
        idx++;

        i += BLEN;
    }

    // padding + dernier bloc
    if (i < len) {
        memset(block, 0, BLEN);
        memcpy(block, m + i, len - i);

        compression(h, block);

        memcpy(out + idx * HLEN, h, HLEN);
        idx++;
    }

    // bloc longueur (pour coherence hash)
    memset(block, 0, BLEN);

    uint64_t bitlen = (uint64_t)len * 8;
    memcpy(block, &bitlen, sizeof(uint64_t));

    compression(h, block);

    memcpy(out + idx * HLEN, h, HLEN);
}