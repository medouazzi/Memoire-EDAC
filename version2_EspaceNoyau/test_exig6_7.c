#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define     DEVICE1_PATH "/dev/mmodule0"

int main() {
    int fd2,fd1;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH; 

// CREATIONS DES DEVICES
    if((fd1 = open(DEVICE1_PATH,O_WRONLY | O_CREAT, mode))==-1)
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
    if((close(fd2))==-1)
        {
        printf("err \n");
        //return -1;
        }

    else
        {
        perror("erreur fermiture \n");
        }
  
return 0;
}
