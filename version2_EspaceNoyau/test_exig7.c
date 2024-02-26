#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define     DEVICE1_PATH "/dev/edac1"

int main() {
    int fd1,fd2,fd3;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH; // Read and write permissions for owner and group, read permission for others

// CREATIONS DES DEVICES
    if((fd2 = open(DEVICE1_PATH,O_WRONLY | O_CREAT, mode))==-1)
        {
        perror("erreur ouvertuere device2");
        //return -1;
        }

    else
        {
        printf("l'indicateur de device2 : %d \n",fd2);
        }
    if((close(fd1))==0)
        {
        printf("device 1 fermé \n");
        //return -1;
        }

    else
        {
        perror("erreur fermiture \n");
        }
return 0;
}
