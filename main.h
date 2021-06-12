#ifndef MAIN_H
#define MAIN_H

#include "libmad/mad.h"

#define POWER_CB_POWER		0x80000000
#define POWER_CB_HOLDON		0x40000000
#define POWER_CB_STANDBY	0x00080000
#define POWER_CB_RESCOMP	0x00040000
#define POWER_CB_RESUME		0x00020000
#define POWER_CB_SUSPEND	0x00010000
#define POWER_CB_EXT		0x00001000
#define POWER_CB_BATLOW		0x00000100
#define POWER_CB_BATTERY	0x00000080
#define POWER_CB_BATTPOWER	0x0000007F

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 272

#define NULL 0

#define O_RDONLY    0x0001 
#define O_WRONLY    0x0002 
#define O_RDWR      0x0003 
#define O_NBLOCK    0x0010 
#define O_APPEND    0x0100 
#define O_CREAT     0x0200 
#define O_TRUNC     0x0400 
#define O_NOWAIT    0x8000 
#define SCE_SEEK_SET	(0)
#define SCE_SEEK_CUR	(1)
#define SCE_SEEK_END	(2)

#define malloc scemalloc
#define free scefree
#define calloc scecalloc
#define memset _memset
#define memcpy _memcpy

#define TABLE_SIZE 64

// memory manager structure
typedef struct _sceMemoryMgr {
  int hBlock;
  unsigned char *ptr;
  int size;
} sceMemoryMgr;

// mp3 file infomation
typedef struct MP3_INFO_ {
  int handle;
  int fd;
  int fsize;
  signed short *buf_wav[2];
  void *buf_mp3;
} MP3_INFO;

extern sceMemoryMgr MemTable[TABLE_SIZE];
extern MP3_INFO minf;

void *scemalloc(int);
void scefree(void *);
void sceMemInit(void);

int mp3play(char *);
#endif
