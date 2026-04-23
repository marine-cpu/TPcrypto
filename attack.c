#include "attack.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>

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
    uint64_t limit=(1ULL<<(HLEN*4));//2¨(n/2)

    // STEP 1 : stock ALL ms first side
    for (count; count < limit; count++) {
        rand_block(ms, BLEN);
        memcpy(tmp, h0, HLEN);
        compression(tmp, ms);
        insert(tmp, ms, ms);

    }
    for (uint64_t i=0;i<limit*4;i++){
        // STEP 2 : try match with mf side
        rand_block(mf, BLEN);
        memcpy(tmp, h0, HLEN);
        compression(tmp, mf);

        count ++;

        entry *e = find(tmp);
        if (e != NULL && memcmp(e->ms,mf,BLEN)!=0) {
            //Collision
            memcpy(hf, tmp, HLEN);
            memcpy(ms, e->ms, BLEN);
            return log2((double)count);
        }
    }
    //failure
    return -1.0;
}

   //STEP 2 : LINK MSG

double linkmsg(byte ml[BLEN], int *idx,const byte hf[HLEN],const byte *h, size_t len){
    uint64_t count = 0;
    uint64_t limit=(1<<20); //limite
    byte tmp[HLEN];
    size_t nb_blocks=len/BLEN;

    while (count<limit) {
        rand_block(ml, BLEN);

        memcpy(tmp, hf, HLEN);
        compression(tmp, ml);

        count++;

        for (size_t i = 0; i < nb_blocks; i++) {
            if (memcmp(tmp, h + i * HLEN, HLEN) == 0) {
                *idx = (int)i;
                return log2((double)count);
            }
        }
    }
    return -1.0;//echec
}

   //STEP 3 : ATTACK

double attack(const byte *m, size_t len, byte *m2)
{
    byte *h=malloc((len/BLEN)*HLEN);
    if(!h) return -1.0;

    intermediate_digests(m,len,h);

    byte ms[BLEN], mf[BLEN], ml[BLEN], hf[HLEN];
    int idx=-1;

    double c1 = collision(ms, mf, hf);
    if(c1<0){printf("Echec de la collision\n");return -1.0;}
    double c2 = linkmsg(ml, &idx, hf, h, len);
    if(c2<0){printf("Echec de linkmsg\n");return -1.0;}

    // construction m2
    size_t pos = 0;

    memcpy(m2 + pos, ms, BLEN); pos += BLEN;
    memcpy(m2 + pos, mf, BLEN); pos += BLEN;
    memcpy(m2 + pos, ml, BLEN); pos += BLEN;

    size_t tail_offset=(idx+1)*BLEN;
    size_t tail_len=len-tail_offset;
    memcpy(m2 + pos, m + tail_offset, tail_len);
    free(h);
    
    return c1 + c2;
}