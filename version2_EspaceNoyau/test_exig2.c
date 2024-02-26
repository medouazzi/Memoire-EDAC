#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define DEVICE1_PATH "/dev/edac1"


int main() {
    int fd1;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH; // Read and write permissions for owner and group, read permission for others

    //création device1

    if((fd1 = creat(DEVICE1_PATH, mode))==-1)
        {
        perror("Failed to create device1");
        return -1;
        }

    else
        {
        printf("l'indicateur de device1 : %d \n",fd1);
        }

    //recréation de device1
    
    if((fd1 = creat(DEVICE1_PATH, mode))==-1)
        {
        perror("device 1 dej& initialisé");
        //return -1;
        }
    else
        {
        printf("l'indicateur de device1 : %d \n",fd1);
        }
    if((fd1 = open(DEVICE1_PATH,O_CREAT, mode))==-1)
        {
        perror("device 1 dej& initialisé");
        //return -1;
        }
    else
        {
        printf("l'indicateur de device1 : %d \n",fd1);
        }

}
