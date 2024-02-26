#include <fcntl.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "edac_ioctl.h"
#include <sys/ioctl.h>
#define DEVICE1_PATH "/dev/mmodule0"

#define WRONG_DEVICE1_PATH "/include/mmodule0"




//                      V2-E3  
//                      V2-E4


int main() {
    int fd1,fd2;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH; 

    //création device1

     if(fd1 = open(WRONG_DEVICE1_PATH,O_RDWR)==-1)
        {
        perror("wrong path");
        //return -1;
        }

    else
        {
        printf("l'indicateur de device1 : %d \n",fd1);
        }
    if(fd1 = open(DEVICE1_PATH,O_RDWR | O_CREAT)==-1)
        {
        perror("erreur ouvertuere device1");
        //return -1;
        }

    else
        {
        printf("l'indicateur de device1 : %d \n",fd1);
        }




}
