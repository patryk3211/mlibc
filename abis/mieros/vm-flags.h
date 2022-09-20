#ifndef _ABIBITS_MMAP_FLAGS_H
#define _ABIBITS_MMAP_FLAGS_H

#define PROT_NONE   0x00
#define PROT_READ   0x01
#define PROT_WRITE  0x02
#define PROT_EXEC   0x04

#define MAP_FAILED ((void*)-1)
#define MAP_PRIVATE     0x00
#define MAP_SHARED      0x01
#define MAP_ANON        0x02
#define MAP_ANONYMOUS   0x02

#endif
