#include "attack.h"
#include <string.h>
#include <math.h>

#define TABLE_SIZE 200003  // premier, assez grand

typedef struct {
    byte key[HLEN];
    byte ms[BLEN];
    byte mf[BLEN];
    int used;
} entry;

static entry table[TABLE_SIZE];

//simple hash
static int hindex(const byte *h)
{
    uint32_t x = 0;
    for (int i = 0; i < HLEN; i++)
        x = (x * 31) ^ h[i];
    return x % TABLE_SIZE;
}

//clear table
static void clear_table()
{
    memset(table, 0, sizeof(table));
}

//insert 
static void insert(const byte *h, const byte *ms, const byte *mf)
{
    int idx = hindex(h);
    while (table[idx].used)
        idx = (idx + 1) % TABLE_SIZE;

    memcpy(table[idx].key, h, HLEN);
    memcpy(table[idx].ms, ms, BLEN);
    memcpy(table[idx].mf, mf, BLEN);
    table[idx].used = 1;
}

//find 
static entry* find(const byte *h)
{
    int idx = hindex(h);
    int start = idx;

    while (table[idx].used) {
        if (memcmp(table[idx].key, h, HLEN) == 0)
            return &table[idx];

        idx = (idx + 1) % TABLE_SIZE;
        if (idx == start) break;
    }
    return NULL;
}

//random block helper 
static void rand_block(byte *b, size_t n)
{
    random_bytes(b, n);
}

   // STEP 1 : COLLISION
double collision(byte ms[BLEN], byte mf[BLEN], byte hf[HLEN])
{
    byte h0[HLEN], tmp[HLEN], dec[HLEN];
    byte zero[HLEN];
    memset(zero, 0, HLEN);

    if (BLOCKSIZE == 32)
        read_bytes("03020100", h0, HLEN);
    else if (BLOCKSIZE == 48)
        read_bytes("050403020100", h0, HLEN);
    else
        read_bytes("0706050403020100", h0, HLEN);

    clear_table();

    uint64_t count = 0;

    // STEP 1 : stock ALL ms first side
    for (;;) {
        rand_block(ms, BLEN);

        memcpy(tmp, h0, HLEN);
        compression(tmp, ms);

        insert(tmp, ms, ms);
        count++;

        // STEP 2 : try match with mf side
        rand_block(mf, BLEN);
        speck_dec(mf, zero, dec);

        count++;

        entry *e = find(dec);
        if (e != NULL) {
            memcpy(hf, dec, HLEN);
            memcpy(ms, e->ms, BLEN);
            return log2((double)count);
        }

        if (count > (1 << 20)) break; // safety
    }

    return log2((double)count);
}

   //STEP 2 : LINK MSG

double linkmsg(byte ml[BLEN], int *idx,const byte hf[HLEN],const byte *h, size_t len){
    uint64_t count = 0;
    byte tmp[HLEN];

    while (1) {
        rand_block(ml, BLEN);

        memcpy(tmp, hf, HLEN);
        compression(tmp, ml);

        count++;

        for (size_t i = 0; i < len / HLEN; i++) {
            if (memcmp(tmp, h + i * HLEN, HLEN) == 0) {
                *idx = i;
                return log2((double)count);
            }
        }
    }
}

   //STEP 3 : ATTACK

double attack(const byte *m, size_t len, byte *m2)
{
    byte h[65536];
    byte ms[BLEN], mf[BLEN], ml[BLEN], hf[HLEN];

    intermediate_digests(m, len, h);

    int idx;
    double c1 = collision(ms, mf, hf);
    double c2 = linkmsg(ml, &idx, hf, h, len);

    // construction m2
    size_t pos = 0;

    memcpy(m2 + pos, ms, BLEN); pos += BLEN;

    memcpy(m2 + pos, mf, BLEN); pos += BLEN;

    for (int i = 0; i < idx; i++) {
        memcpy(m2 + pos, mf, BLEN);
        pos += BLEN;
    }

    memcpy(m2 + pos, ml, BLEN); pos += BLEN;

    memcpy(m2 + pos, m + idx * BLEN, len - idx * BLEN);
    pos += (len - idx * BLEN);

    return c1 + c2;
}