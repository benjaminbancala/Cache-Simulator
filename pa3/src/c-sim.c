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
long CacheHits=0;
long CacheMisses=0;
long MemoryReads=0;
long MemoryWrites=0;
long globalOpCount=0;
void assoc_Op(char* op, char* writePolicy, block cache2[], long cacheTag, long setSize){
	bool hit = false;
	int index;
	int r;
	int through;
	int i;
	int LRU;
	globalOpCount++;
	for(index =0;index<setSize;index++){
		if(cache2[index].tag==cacheTag){
			hit=true;
			break;
		}
	}
	r=strcmp("R",op);
	through=strcmp("wt",writePolicy);
	if(hit){
		CacheHits++;
		cache2[index].accessCount=globalOpCount;
		if(r==0){	
		}else{
			if(through==0)
				MemoryWrites++;
			cache2[index].dirty=1;
		}
	}else{
		CacheMisses++;
		MemoryReads++;
		LRU=0;
		for(i=0;i<setSize-1;i++){
			if(cache2[LRU].accessCount>cache2[i+1].accessCount)
				LRU=i+1;
			}
			if(r==0){
				if(cache2[LRU].dirty==1){
					if(through!=0)
						MemoryWrites++;
				}
			cache2[LRU].tag=cacheTag;
			cache2[LRU].dirty=0;
			cache2[LRU].accessCount=globalOpCount;
			}else{
				if(cache2[LRU].dirty==1||through==0){
					MemoryWrites++;
				}
			cache2[LRU].tag=cacheTag;
			cache2[LRU].dirty=1;
			cache2[LRU].accessCount=globalOpCount;
			}
		}
}
void direct_Op(int blockBits, char* op, char* assoc, char* writePolicy, block cache[], long address, long cacheEntries){
	int r=strcmp("R",op);
	int w=strcmp("W",op);
	int through=strcmp("wt",writePolicy);
	long addressTag;
	long cacheAddress;
	addressTag=address>>blockBits;
	cacheAddress=addressTag%cacheEntries;
	if(r==0){
		if(cache[cacheAddress].tag==addressTag){
		CacheHits++;
	}else{
		CacheMisses++;
		MemoryReads++;
		if(cache[cacheAddress].dirty==1){
			if(through!=0)
				MemoryWrites++;
		}
		cache[cacheAddress].dirty=0;
		cache[cacheAddress].tag=addressTag;
	}
	return;
}
	if(w==0){
		if(cache[cacheAddress].tag==addressTag){
			CacheHits++;
			cache[cacheAddress].dirty=1;
			if(through==0)
				MemoryWrites++;
		}else{
			if(cache[cacheAddress].dirty==1||through==0){
				MemoryWrites++;
			}
			CacheMisses++;
			MemoryReads++;
			cache[cacheAddress].dirty=1;
			cache[cacheAddress].tag=addressTag;
		}
	return;
	}
}
int main (int argc, char* argv[]){
	char* cachesize;
	int cacheSize;
	char* assoc;
	int direct;
	char *blocksize;
	int blockSize;
	int blockBits;
	long cacheElements;
	int i;
	int k;
	char* write;
	bool wt;
	char *filename;
	FILE *fp;
	int c;
	char tok1[100];
	char tok2[100];
	char tok3[100];
	long address;
	int full;
	int setNum;
	char* word;
	long setSize;
	int j;
	long temp;
	long setIndex;
	long cacheTag;
	if(argc!=6){
		printf("invalid number of command arguements");
		return 0;
	}
	cachesize=argv[1];
	cacheSize=atoi(cachesize);
	assoc=argv[2];
	direct=strcmp("direct",assoc);
	fflush (stdout);	
	blocksize=argv[3];
	blockSize=atoi(blocksize);
	blockBits=(int)log2(blockSize);
	if(direct==0){
		cacheElements=(long)cacheSize/blockSize;
		block cache[cacheElements];
			for(i=0;i<cacheElements;i++){
				cache[i].tag=-1;
				cache[i].dirty=0;
				cache[i].accessCount=0;
			}
		write=argv[4];
		fflush(stdout);
		k = strcmp(&write[0],"wt");
		if (k==0){
			wt=true;
		}else{
			wt=false;
		}
		filename=argv[5];
		fp=fopen(filename, "r");
		do{
			c=fscanf(fp,"%s",tok1);
			c=fscanf(fp,"%s",tok2);
			c=fscanf(fp,"%s",tok3);
			if(c==EOF)
				continue;
			address=strtol(tok3,(char**)NULL,16);
			fflush(stdout);
			direct_Op(blockBits,tok2,assoc,write,&cache[0],address,cacheElements);
		}while(c!=EOF);
	
	printf("Memory Reads:%ld\n", MemoryReads);
	printf("Memory Writes:%ld\n", MemoryWrites);
	printf("Cache Hits:%ld\n",CacheHits);
	printf("Cache Misses%ld\n",CacheMisses);
		return 0;
	}else{
	full=strcmp("assoc",assoc);
	if(full==0){
		setNum=1;
	}else{
		word=strtok(assoc,":");
		word=strtok(NULL,":");
		setNum=atoi(word);
	}
	fflush(stdout);
	setSize=(cacheSize/(blockSize*setNum));
	block cache2[setNum][setSize];
	for(i=0; i<setNum; i++){
		for(j=0;j<setSize;j++){
			cache2[i][j].tag=-1;
			cache2[i][j].dirty=0;
			cache2[i][j].accessCount=0;
		}
	}
		write=argv[4];
	fflush(stdout);
	k=strcmp(&write[0],"wt");
	if(k==0){
		wt=true;
	}else{
		wt=false;
	}
	filename=argv[5];
	fp=fopen(filename, "r");
	do{
	c=fscanf(fp,"%s",tok1);
		c=fscanf(fp,"%s",tok2);
		c=fscanf(fp,"%s",tok3);
		if(c==EOF)
			continue;
		address=strtol(tok3,(char**)NULL,16);
		temp=address>>blockBits;
		setIndex=temp%setNum;
		cacheTag=temp>>(long)log2(setNum);
		assoc_Op(tok2, write, &cache2[setIndex][0],cacheTag,setSize);
		fflush(stdout);

	}while(c!=EOF);
	printf("Memory Reads: %ld\n", MemoryReads);
	printf("Memory Writes: %ld\n",MemoryWrites);
	printf("Cache Hits: %ld\n", CacheHits);
	printf("Cache Misses: %ld\n",CacheMisses);
}
}




		
