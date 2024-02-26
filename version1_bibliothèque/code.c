#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include "hamming.h"
#include "code.h"

/*
typedef struct pthread_edac_t {
    unsigned char* memory;
    int memorysize;
    pthread_mutex_t mutex;
} pthread_edac_t;*/

/*unsigned char generate_random_byte() {
    return (unsigned char)(rand() % 256);
}*/

int pthread_edac_init(struct pthread_edac_t* edac, int memorysize) {
    if (edac == NULL || memorysize <= 0) {
        errno = EINVAL;
        return -1;
    }

    int mutex_init_result = pthread_mutex_init(&(edac->mutex), NULL);
    if (mutex_init_result != 0) {
        return mutex_init_result;  // Erreur lors de l'initialisation du mutex
    }

    edac->memory = (unsigned char*)malloc(memorysize);


    if (edac->memory == NULL) {
        errno = ENOMEM;
        pthread_mutex_destroy(&edac->mutex);
        return -1;
    }
    edac->memorysize = memorysize;
    return 0;
}

int pthread_edac_destroy(struct pthread_edac_t* edac) {
    if (edac == NULL) {
        errno = EINVAL;
        return -1;
    }
    free(edac->memory);
    int mutex_destroy_result = pthread_mutex_destroy(&edac->mutex);
    if (mutex_destroy_result != 0) {
        return mutex_destroy_result;  // Erreur lors de la destruction du mutex
    }
    return 0;
}

int pthread_edac_write(struct pthread_edac_t* edac, unsigned char* data, int size, int offset) {
    if (edac == NULL || data == NULL || size <= 0 || offset < 0 ) {
        errno = EINVAL;
        return -1;
    }
    int mutex_lock_result = pthread_mutex_lock(&(edac->mutex));
    if (mutex_lock_result != 0) {
        return mutex_lock_result;  // Erreur lors du verrouillage du mutex
    }
    if (offset + size*2 > edac->memorysize) {
        size = (edac->memorysize - offset)/2;  // Adjust size to fit within the allocated memory
    }
     unsigned char encodedDataFirst4Bits ;
    unsigned char encodedDataNext4Bits;
    printf("Thread ID: %lu,: \n write data :", (unsigned long)pthread_self());
    for(int i=0 ; i<size ;i++){

            encodedDataNext4Bits = HammingMatrixEncode((data[i] >> 4));
            memcpy(edac->memory + offset + i*2, &encodedDataNext4Bits, 1);
            printf("0x%02X \t", encodedDataNext4Bits);
            encodedDataFirst4Bits = HammingMatrixEncode(data[i]);
            memcpy(edac->memory + offset + i*2 + 1, &encodedDataFirst4Bits, 1);
            printf("0x%02X \t", data[i]);
       
}
    
    printf( "\n");

    int mutex_unlock_result = pthread_mutex_unlock(&(edac->mutex));
    if (mutex_unlock_result != 0) {
        return mutex_unlock_result;  
    }
    return size;
}

    int pthread_edac_read(struct pthread_edac_t* edac, unsigned char* data, int size, int offset) {
    if (edac == NULL || data == NULL || size <= 0 || offset < 0) {
        errno = EINVAL;
        return -1;
    }
    int mutex_lock_result = pthread_mutex_lock(&(edac->mutex));
    if (mutex_lock_result != 0) {
        return mutex_lock_result; 
    }

     if (offset + size*2 > edac->memorysize) {
        size = (edac->memorysize - offset)/2; 
    }
    unsigned char decodedDataNext4Bits ;
    unsigned char decodedDataFirst4Bits;
    unsigned char corruptedDataNext4Bits ;
    unsigned char corruptedDataFirst4Bits;
    printf("Thread ID: %lu, Read Data: \n ", (unsigned long)pthread_self());
    for(int i=0 ; i<size ;i++){
            memcpy(&corruptedDataNext4Bits, edac->memory + offset+i*2, 1);
         
            decodedDataNext4Bits = HammingMatrixDecode(corruptedDataNext4Bits);
        
            memcpy(&corruptedDataFirst4Bits, edac->memory + offset+i*2+1, 1);
        
            decodedDataFirst4Bits = HammingMatrixDecode(corruptedDataFirst4Bits);

            data[i] = (decodedDataNext4Bits << 4) | decodedDataFirst4Bits;
            printf("0x%02X \t", data[i]);                   
}
    
    

   
    printf("\n");

    // Unlock the mutex
    int mutex_unlock_result = pthread_mutex_unlock(&(edac->mutex));
    if (mutex_unlock_result != 0) {
        return mutex_unlock_result;  // Error unlocking the mutex
    }

    return size;  // Return the actual number of bytes read
}

/*
void* fonction(void* arg) {
   // pthread_edac_t* edac = (pthread_edac_t*)arg;

    unsigned char data_to_write[15];
     for (int i = 0; i < 15; ++i) {
        data_to_write[i] = 0x11*i ;  // Filling data with values 1, 2, 3, ..., 15
    }
  
    int size = pthread_edac_write((pthread_edac_t*)arg, data_to_write, sizeof(data_to_write), 0);
    printf("the number of characters actually written : %d \n",size);
    

    return NULL;
}
void* fonction2(void* arg) {
   // pthread_edac_t* edac = ;
 /*
    unsigned char write_data = generate_random_byte();
    printf("Thread ID: %lu, write Data: %u\n", (unsigned long)pthread_self(), write_data);
    pthread_edac_write(edac, &write_data, sizeof(write_data), 0);

    unsigned char read_data[15];
    int size =pthread_edac_read((pthread_edac_t*)arg, read_data, sizeof(read_data), 0);
    printf("the number of characters actually reading : %d \n",size);


    



    return NULL;
}

int main() {



    pthread_edac_t edac;

    if (pthread_edac_init(&edac, 12) != 0) {
        return 1;
    }
    
    const int num_threads = 5; 
    pthread_t threads[num_threads];

 
    pthread_create(&threads[0], NULL, fonction, &edac);
    
    pthread_create(&threads[2], NULL, fonction2, &edac);
    
    pthread_create(&threads[1], NULL, fonction, &edac);
    
    pthread_create(&threads[3], NULL, fonction2, &edac);

pthread_join(threads[0], NULL);
pthread_join(threads[1], NULL);
pthread_join(threads[2], NULL);
    pthread_join(threads[3], NULL);
   /* for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
    }
    
    
    
    
 

    pthread_edac_destroy(&edac);

    return 0;
}*/

