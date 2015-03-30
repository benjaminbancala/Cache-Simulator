#ifndef _C-SIM_H
#define _C_SIM_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>


typedef struct block{
	long tag;
	int dirty;
	long accessCount;
}block;

void assoc_Op(char* op, char*writePolicy, block cache2[], long cacheTag, long setSize);

void direct_Op(int blockBits, char* op, char* writePolicy, block cache[], long address, long cacheEntries);

#endif
