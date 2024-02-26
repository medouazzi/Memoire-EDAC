#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include "hamming.h"
#include  <string.h>
#include "bib.h" 


void* fonction(void* arg) {
   

    unsigned char data_to_write[15];
     for (int i = 0; i < 15; ++i) {
        data_to_write[i] = 0x11*i ; 
    }
  
    int size = pthread_edac_write((pthread_edac_t*)arg, data_to_write, sizeof(data_to_write), 0);
    printf("the number of characters actually written : %d \n",size);
    

    return NULL;
}
void* fonction2(void* arg) {
  

    unsigned char read_data[15];
    int size = pthread_edac_read((pthread_edac_t*)arg, read_data, sizeof(read_data), 0);
    printf("the number of characters actually reading : %d \n",size);

    return NULL;
}

void inject_faults(struct pthread_edac_t* edac) {
	    srand((unsigned int)time(NULL));

	        for (int i = 0; i < 4; i++) {
			int random_index = rand() % edac->memorysize;
			int random_bit = rand() % 8;
			edac->memory[random_index] ^= (1 << random_bit);
			printf("Fault injected at byte %d, bit %d\n", random_index, random_bit);
			printf("Data after injection: ");
						
			for (int j = 0; j < edac->memorysize; j++) {
										            					printf("0x%02X\t", (unsigned char)edac->memory[j]);
											            }
				usleep(10000);
		}

		    printf("Fault injection process stopped.\n");
}

int main() {
    
    srand(time(NULL));
    pthread_edac_t edac;

    pthread_edac_init(&edac, 12); 
    
    pid_t pid = fork();

    if (pid == -1) {
        perror("Erreur lors de la création du processus fils");
        exit(1);
    } else if (pid == 0) {
        
        printf("Je suis le processus fils (PID : %d)\n", getpid());

        
        inject_faults(&edac);

       
        
    } else {
        
        printf("Je suis le processus parent (PID : %d), mon fils a le PID : %d\n", getpid(), pid);

        
    
    const int num_threads = 5; 
    pthread_t threads[num_threads];
     if (pthread_edac_init(&edac, 12) != 0) {
	             return 1;
		     printf("howhow \n");
		         }

   
   int status= pthread_create(&threads[0], NULL, fonction, &edac);
   if(status!=0)printf("whywhy");
    usleep(10000);
    pthread_create(&threads[2], NULL, fonction2, &edac);

    printf("mzahiach \n ");
    usleep(10000);
    pthread_create(&threads[1], NULL, fonction2, &edac);
    usleep(10000);
    pthread_create(&threads[3], NULL, fonction2, &edac);

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    pthread_join(threads[2], NULL);
    pthread_join(threads[3], NULL);
   
        
    pthread_edac_destroy(&edac);

    printf("dazt");    
        
    }

    return 0;
}

