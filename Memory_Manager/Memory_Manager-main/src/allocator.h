#ifndef __MY_LIB_H__
#define __MY_LIB_H__
#include <stdio.h>
#include <unistd.h>
#include"deque.h"
// #include <math.h>
#include <sys/mman.h>
#include <threads.h>

#include <string.h>
#include<stdlib.h>
// #include <dlfcn.h>

// = []()-> long unsigned int{
//    long unsigned int N =(long unsigned int) (sizeof(pageblock));
//     if (!(N & (N - 1)))
//         return N;
//     // else set only the left bit of most significant bit
//     return (0x8000000000000000 >> (__builtin_clzll(N) - 1));
// };


#define PAGE_NUM 8
#define CLASS_NUMBER 128 // 2048/min_object_size
#define MINIMUM_OBJECT_SIZE 16
#define MAXIMUM_OBJECT_SIZE 2048
#define PAGE_SIZE 4096
#define BASE_SIZE 127

// #define CLEAN_FIRST_16_BYTES_OF_POINTER(X)(X & (((~(0ULL))<<16)>>16) )
#define LOG2(X) ((unsigned) (8*sizeof (unsigned long long) - __builtin_clzll((X)) - 1))

#define NEAREST_POWER_OF_2_NUMBER(X)( (!(X & (X - 1))) ? X: (0x8000000000000000 >> (__builtin_clzll(X) - 1)))
#define PAGE_BLOCK_ALIGNMENT(X)((X/MINIMUM_OBJECT_SIZE)+MINIMUM_OBJECT_SIZE)

#define MASK_SIZE LOG2(PAGE_NUM*PAGE_SIZE)


    static const int base[]={16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 
                            208, 224, 240, 256, 272, 288, 304, 320, 336, 352, 368, 384, 
                            400, 416, 432, 448, 464, 480, 496, 512, 528, 544, 560, 576, 
                            592, 608, 624, 640, 656, 672, 688, 704, 720, 736, 752, 768, 
                            784, 800, 816, 832, 848, 864, 880, 896, 912, 928, 944, 960, 
                            976, 992, 1008, 1024, 1040, 1056, 1072, 1088, 1104, 1120, 1136, 1152, 
                            1168, 1184, 1200, 1216, 1232, 1248, 1264, 1280, 1296, 1312, 1328, 1344, 
                            1360, 1376, 1392, 1408, 1424, 1440, 1456, 1472, 1488, 1504, 1520, 1536, 
                            1552, 1568, 1584, 1600, 1616, 1632, 1648, 1664, 1680, 1696, 1712, 1728, 
                            1744, 1760, 1776, 1792, 1808, 1824, 1840, 1856, 1872, 1888, 1904, 1920, 
                            1936, 1952, 1968, 1984, 2000, 2016, 2032,2048

                        };

    static const unsigned int alignment[]={1024,1024,1024,1024,1024,1024,1024, 4096, 4096, 4096, 4096, 4096, 4096, 
                                4096, 4096, 4096, 4096,4096, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 
                                8192, 8192, 8192, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 
                                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 
                                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 
                                16384, 16384, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 
                                32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 
                                32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 
                                32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 65536, 65536, 65536, 65536, 
                                65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 
                                65536, 65536, 65536, 65536, 65536, 65536,65536
                                };

// static const unsigned int alignment[]={32768,32768,32768,32768,32768,32768,32768, 32768, 32768, 32768, 32768, 32768, 32768, 
//                                 32768, 32768, 32768, 32768,32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 
//                                 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 
//                                 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 
//                                 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 
//                                 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 
//                                 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 
//                                 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 
//                                 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 
//                                 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 
//                                 32768, 32768, 32768, 32768, 32768, 32768,32768
//                                 };



typedef struct superpage_struct{

struct superpage_struct* next;
struct superpage_struct* prev;



// struct s_deque
struct s_deque* list;

}superpage;



typedef struct pageblock_struct{

    // struct superpage* sp; //point to my superpage 

    struct pageblock_struct *next;
    struct pageblock_struct *prev;
    
    unsigned long unallocated_offset;// points to first unallocated object
    
    unsigned long free_list_offset; // points to first free, recycled object
    
    // unsigned long mem_size; // size in bytes of the space
    
    // unsigned int class_size; // size in bytes of all objects in the block
    unsigned int class_id;
    
    int num_free_objects;// total number of free objects

    int num_alloc_objects; // total number of allocated objects

  
    unsigned int thread_id;
    unsigned int is_orphan;

    t_stack remote_stack;

    // heap my_heap;     

    // char tpt[5];
    // char* mem; //points to the beginning of our memory


}pageblock;

typedef struct heap_struct{

    pageblock *head,*tail;

}heap;

void* get_memory(int class_id);

int retrieve_class_id(size_t size);

void* page_block_allocation(pageblock* page_b,int class_id);

void* large_object_allocation(size_t size);

int is_large_object(void *ptr);

void large_object_deallocation(void* ptr);

char* get_large_object_real_address(void* ptr);

int is_double_free(void* ptr);
void set_freed_object_header(void* ptr);

void adopt_pageblock(pageblock* pg_block, void* ptr);

size_t get_large_object_size(void* ptr);

pageblock* init_page_block( int class_id);

pageblock* get_page_block_start(void*ptr);
void declare_pageblock_orphan( pageblock* ptr);
void remote_free(pageblock* page, void* ptr);

#ifdef __cplusplus
    #define _BEGIN_DECLS_ extern "C" {
    #define _END_DECLS_         }
#else
    #define _BEGIN_DECLS_
    #define _END_DECLS_
#endif
_BEGIN_DECLS_

void* malloc(size_t size);
void *realloc(void *ptr, size_t sz);
void *calloc(size_t nmemb, size_t sz);
void free(void* ptr);
 
_END_DECLS_
#endif