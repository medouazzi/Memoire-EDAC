#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

#define SHM_KEY 1234,

void inject_faults(char* data, int size) {
 int i=0;
 srand((unsigned int)time(NULL));
 while(i<4){
 int random_index = rand() % size;
 int random_bit = rand() % 8;

     data[random_index] ^= (1 << random_bit);
     printf("Fault injected at byte %d, bit %d\n", random_index, random_bit);
 printf("Data after injection: ");
         for (int j = 0; j < size; j++) {
		             printf("%02X ", (unsigned char)data[j]);
			             }
	         printf("\n");
 i++;
 }
if (i== 4) {
	            printf("Fault injection process stopped.\n");
		                exit(0);
				                                        }

}

                                                                              int main() {
										          
										          srand(time(NULL));

											      
											      int shm_id = shmget(SHM_KEY, sizeof(int), IPC_CREAT | 0666);
											          if (shm_id == -1) {
													          perror("Erreur lors de la création/accès au segment de mémoire partagé");
														          exit(1);
															      }

												      
												      int *edac_memory = (int *)shmat(shm_id, NULL, 0);
												          if ((intptr_t)edac_memory == -1) {
														          perror("Erreur lors de l'attachement du segment de mémoire partagé");
															          exit(1);
																      }

													     
													      *edac_memory = 0;

													          
													          pid_t pid = fork();

														      if (pid == -1) {
															              perror("Erreur lors de la création du processus fils");
																              exit(1);
																	          } else if (pid == 0) {
																			          
																			          printf("Je suis le processus fils (PID : %d)\n", getpid());

																				          
																				          inject_faults((char *)edac_memory, sizeof(int));

																					          
																					          shmdt(edac_memory);
																						      } else {
																							             
																							              printf("Je suis le processus parent (PID : %d), mon fils a le PID : %d\n", getpid(), pid);

																								              
																								              wait(NULL);

																									              
																									              printf("Valeur de la mémoire EDAC après la simulation de fautes : %d\n", *edac_memory);

																										              
																										              shmdt(edac_memory);

																											              
																		              shmctl(shm_id, IPC_RMID, NULL);
																												          }

														          return 0;
									      }


