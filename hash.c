#include <stdio.h>
#include "utils.h"

void compression(byte h[HLEN], const byte m[BLEN]){
    byte c[MLEN];
    for (int i=0;i<BLEN;i++){
        speck_enc(m[i],h[i],c);
        if (i>0)
           h[i]=c^h[i-1];
    }
};
void hash(const byte *m, size_t len, byte h[HLEN]){
    
};
void intermediate_digests(const byte *m, size_t len, byte *h){

};