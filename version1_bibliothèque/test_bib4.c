#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include "bib.h"




void* fonction(void* arg) {
   // pthread_edac_t* edac = (pthread_edac_t*)arg;

    unsigned char data_to_write[17];
     for (int i = 0; i < 17; ++i) {
        data_to_write[i] = 0x11*i ;  // Filling data with values 1, 2, 3, ..., 15
    }
   // int size = pthread_edac_write((pthread_edac_t*)arg, data_to_write, sizeof(data_to_write), 0);  
    int size = pthread_edac_write((pthread_edac_t*)arg, data_to_write, sizeof(data_to_write),10);
    if (size == -1) {
   perror("Erreur lors de l'ecriture");
        return NULL;
    }

    printf("the number of characters actually written : %d \n",size);
    

    return NULL;
}
void* fonction2(void* arg) {
 

    unsigned char read_data[15];
  //int size =pthread_edac_read((pthread_edac_t*)arg, read_data, sizeof(read_data), 0);
  //  int size =pthread_edac_read((pthread_edac_t*)arg, read_data, sizeof(read_data), 4);
    
   int size =pthread_edac_read((pthread_edac_t*)arg, read_data, sizeof(read_data), -5);
      if (size == -1) {
   perror("Erreur lors de la lecture");
        return NULL;
    }

    printf("the number of characters actually reading : %d \n",size);


    



    return NULL;
}

int main() {



    pthread_edac_t edac;



    
    if (pthread_edac_init(&edac,10) != 0) {
        return 1;
    }
    
    const int num_threads = 5; 
    pthread_t threads[num_threads];

 /*    if (pthread_edac_init(NULL, 5000) != 0) {
perror("Erreur lors de l'initialisation de pthread_edac_init");
        return 1;
    }*/

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
    }*/
    
    
    
    
 

    pthread_edac_destroy(&edac);

    return 0;
}
