#ifndef BIB_H
#define BIB_H

#define DATA_BITS 4
#define CODE_BITS 7
#define PARITY_BITS 3
#define DATA_VALUES (1 << DATA_BITS)
#define CODE_VALUES (1 << CODE_BITS)
#define PARITY_VALUES (1 << PARITY_BITS)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>


typedef struct pthread_edac_t {
    int shm_id;
    unsigned char* memory;
    int memorysize;
    pthread_mutex_t mutex;
} pthread_edac_t;


//unsigned char generate_random_byte();
int pthread_edac_init(struct pthread_edac_t* edac, int memorysize);
int pthread_edac_destroy(struct pthread_edac_t* edac);
int pthread_edac_write(struct pthread_edac_t* edac, unsigned char* data, int size, int offset);
int pthread_edac_read(struct pthread_edac_t* edac, unsigned char* data, int size, int offset);
void* fonction(void* arg);
void* fonction2(void* arg);
unsigned char HammingMatrixEncode(unsigned char data);
unsigned char HammingMatrixDecode(unsigned char code);

#endif 

