// libcŽü‚è‚ÌƒJƒvƒZƒ‹ŠÖ”ŒQ
// 
#include "main.h"

void* _memcpy(void *buf1, const void *buf2, int n)
{
  while(n-->0)
    ((unsigned char*)buf1)[n] = ((unsigned char*)buf2)[n];
  return buf1;
}

void* _memset(void *buf, int ch, int n)
{
  unsigned char *p = buf;
  
  while(n>0)
    p[--n] = ch;
  
  return buf;
}

void sceMemInit(void)
{
  int i;

  for(i=0;i<TABLE_SIZE;i++) {
    MemTable[i].ptr = 0;
    MemTable[i].hBlock = 0;
  }
}

void *scemalloc(int size)
{
  int i;

  for(i=0;i<TABLE_SIZE;i++) {
    if(MemTable[i].hBlock == 0) {
      break;
    }
  }

  if(i < TABLE_SIZE) {
    MemTable[i].hBlock = sceKernelAllocPartitionMemory(2, "block", 0, size);
    MemTable[i].ptr = (unsigned char *)sceKernelGetBlockHeadAddr(MemTable[i].hBlock);
    MemTable[i].size = size;
    return MemTable[i].ptr;
  } else {
    return NULL;
  }
}

void *scecalloc(int size)
{
  void *ptr;
  
  ptr = scemalloc(size);
  if(ptr != NULL) {
    memset(ptr, 0, size);
  }
  
  return ptr;
}

void scefree(void *ptr)
{
  int i;
  
  for(i=0;i<TABLE_SIZE;i++) {
    if(MemTable[i].ptr == ptr) {
      sceKernelFreePartitionMemory(MemTable[i].hBlock);
      MemTable[i].hBlock = 0;
      MemTable[i].ptr = 0;
      MemTable[i].size = 0;
      return;
    }
  }
}
