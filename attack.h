#ifndef ATTACK_H
#define ATTACK_H

#include <stdint.h>
#include <stddef.h>
#include "utils.h"
#include "hash.h"

double collision(byte ms[BLEN], byte mf[BLEN], byte hf[HLEN]);

double linkmsg(byte ml[BLEN], int *i,const byte hf[HLEN],const byte *h, size_t len);

double attack(const byte *m, size_t len, byte *m2);

#endif