#include <stdio.h>
#include "utils.h"



int main(int argc, char* argv[]) {
    byte h[HLEN];
    if (BLOCKSIZE== 32)
        h[0] = "0x03020100";
    else if (BLOCKSIZE== 48)
        h[0] = "0x050403020100";
    else if (BLOCKSIZE== 64)
        h[0] = "0x0706050403020100";
    const byte  m = "0123456789abcdef";
    compression(h[0], m);
    return 0;

}
