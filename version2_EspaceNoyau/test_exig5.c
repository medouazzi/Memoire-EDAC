#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define     DEVICE1_PATH "/dev/mmodule0"
#define     DEVICE2_PATH "/dev/mmodule0"
#define     DEVICE3_PATH "/dev/mmodule0"
#define     NO_VALIDE 0
int main() {
    int fd1,fd2,fd3;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH; 



     if((fd1 = open(DEVICE1_PATH,O_RDONLY | O_CREAT, mode))==-1)
        {
        perror("wrong path");
        //return -1;
        }

    else
        {
        printf("l'indicateur de device2 : %d \n",fd1);
        }
    if((fd2 = open(DEVICE2_PATH,O_WRONLY | O_CREAT, mode))==-1)
        {
        perror("erreur ouvertuere device2");
        //return -1;
        }

    else
        {
        printf("l'indicateur de device2 : %d \n",fd2);
        }
    if((fd3 = open(DEVICE3_PATH, O_APPEND , mode))==-1)
        {
        perror("erreur ouvertuere device3");
        //return -1;
        }

    else
        {
        printf("l'indicateur de device3 : %d \n",fd3);
        }
    if((fd3 = open(DEVICE3_PATH,O_RDWR | O_CREAT, mode))==-1)
        {
        perror("erreur ouvertuere device3");
        //return -1;
        }

    else
        {
        printf("l'indicateur de device3 : %d \n",fd3);
        }



}
