/*************/
/* Atah Omar */
/* 101865123 */
/*************/
/*
Got help from tutors
https://www.youtube.com/watch?v=HFa1NkFTeSw
Help from other class mates
*/

#include "cachelab.h"
#include <assert.h>
#include <limits.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define BUFFER_SIZE UINT_MAX

//counters
int hit_count = 0;
int miss_count = 0;
int eviction_count = 0;
unsigned long long int lru_count = 1;
//args
int s = 0;//set index bits
int b = 0;//block bits
int E = 0;//# of lines per set
char* traceFile = NULL;//trace file
int verboseFlag = 0;//flag to display trace info

//Calculation vars
int S;//2^s
int B;//2^b

//cacheLine
typedef struct cacheLine
{
  char validTag;
  unsigned long long tag;
  unsigned long long lru;
}cacheLine_t;

typedef cacheLine_t* cacheSet_t;//the set of lines
typedef cacheSet_t* cache_t;//the whole cache full of sets

cache_t cache;//Our cache 
unsigned long long int  setMask;//the mask we use


//power function because it kept giving me errors when trying to link math library
int  power(int base,int exponent){
  int res = 1;
  while(exponent)
    {
      if(exponent & 1)
        {
          res *= base;
        }
      exponent >>= 1;
      base *= base;
    }
  return res;
}
//Function to allocate memory for the cache using malloc
void allocateMem()
{
  cache = (cacheSet_t*)malloc(sizeof(cacheSet_t)*S);
  for(int i = 0;i<S;i++)
    {
      cache[i] = (cacheLine_t*)malloc(sizeof(cacheLine_t)*E);
      for(int j = 0;j<E;j++)
	{
	  //Initialize values of cache to 0
	  cache[i][j].validTag = 0;
	  cache[i][j].tag = 0;
	  cache[i][j].lru = 0;
	}
    }
  //set the set mask to S-1
  setMask = (unsigned long long int)(power(2,s)-1);//S = 2^s
}
//The actual simulation of the cache   
void simulationCache(unsigned long long int address)
{
  unsigned int evicLine = 0;//The index of the line we evict
  unsigned long long int eviction = ULONG_MAX;//This is the lru for the evicted line

  unsigned long long int setIndex = (address >> b) & setMask;//index of the set
  unsigned long long int tag = address >> (s + b);//tag value

  cacheSet_t cacheSet = cache[setIndex];//the set we are working on

  for(int i = 0;i<E;i++)
    {
      if(cacheSet[i].validTag)
	{
	  if(cacheSet[i].tag == tag)
	    {
	      cacheSet[i].lru = lru_count++;
	      hit_count++;// if the cache is there increase the hit count
	      return;
	    }
	}
    }
  miss_count++;//if not increase miss count

  for(int i = 0;i < E;i++)
    {
      if(eviction > cacheSet[i].lru)//if the current eviction lru is > than the lru of the current set
	{
	  eviction = cacheSet[i].lru;//new eviction lru is the current set lru
	  evicLine = i;//set the index of the evicted line to the the i val
	}
    }
  if(cacheSet[evicLine].validTag)
    {
      eviction_count++;
    }

  cacheSet[evicLine].validTag = 1;//set the current set vaild tag
  cacheSet[evicLine].tag = tag;//actual tag
  cacheSet[evicLine].lru = lru_count++;//increase the lru counter
}


void usage(char* argv[])
{
  printf("Usage: ./%s [-hv] -s <s> -E <E> -b <b> -t <traceFile>\n",argv[0]);
  printf("-h: Optional help flag that prints usage info\n");
  printf("-v: Optional verbose flag that displays trace info\n");
  printf("-s <s>: Number of set index bits (S = 2^s is the number of sets)\n");
  printf("-E <E>: Associativity (number of lines per set)\n");
  printf("-b <b>: Number of black bits(B = 2^b is the black size)\n");
  printf("-t <traceFile>: Name of the valgrind trace to replay\n");
}

void verbose(FILE* filename)
{
  char c;
  while(!feof(filename))
    {
      c = fgetc(filename);
      printf("%c",c);
    }
  fclose(filename);
}

int main(int argc,char* argv[])
{
  unsigned long long int address;
  char c;
  
  while((c = getopt(argc,argv, "s:E:b:t:vh")) != -1)
    {
      switch(c)
	{
	case 's':
	  s = atoi(optarg);
	  break;
	case 'b':
	  b = atoi(optarg);
	  break;
	case 'E':
	  E = atoi(optarg);
	  break;
	case 't':
	  traceFile = optarg;
	  break;
	case 'v':
	  verboseFlag = 1;
	  break;
	case 'h':
	  usage(argv);
	  exit(0);
	default:
	  exit(1);
	}
    }

  S =(unsigned int) power(2,s);
  B =(unsigned int) power(2,b);

  allocateMem();

  //Trace file setup
  FILE* file = fopen(traceFile,"r");
  char trace_c;
  int size;

  while(fscanf(file," %c %llx,%d",&trace_c,&address,&size)==3)
    {
      switch(trace_c)
	{
	case 'L':
	  simulationCache(address);
	  break;
	case 'M':
	  simulationCache(address);
	  simulationCache(address);
	  break;
	case 'S':
	  simulationCache(address);
	  break;
	default:
	  break;
	}
    }

  if(verboseFlag == 1)
    {
      verbose(file);
    }
  
  
  //printf("%d\n",verboseFlag);
  printSummary(hit_count,miss_count,eviction_count);
  return 0;
}
