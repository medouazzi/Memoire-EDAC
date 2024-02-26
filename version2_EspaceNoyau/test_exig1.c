#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define DEVICE1_PATH "/dev/edac1"
#define DEVICE2_PATH "/dev/edac2"
#define DEVICE3_PATH "/dev/edac3"
#define DEVICE3_PATH "/dev/edac4"
#define DEVICE3_PATH "/dev/edac5"

int main() {
    int fd1,df2,fd3,fd4,fd5;
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

    //création device2
    
    if((fd2 = creat(DEVICE2_PATH, mode))==-1)
        {
        perror("Failed to create  device2");
        return -1;
        }
    else
        {
        printf("l'indicateur de device2 : %d \n",fd2);
        }

    //création device3

    if((fd3 = creat(DEVICE3_PATH, mode))==-1)
        {
        perror("Failed to create device3");
        return -1;
        }
    else
        {
        printf("l'indicateur de device3 : %d \n",fd3);
        }

    //création device4

    if((fd4 = creat(DEVICE4_PATH, mode))==-1)
        {
        perror("Failed to create device4");
        return -1;
        }
    else
        {
        printf("l'indicateur de device4 : %d \n",fd4);
        }

    //création device5

    if((fd5 = creat(DEVICE5_PATH, mode))==-1)
        {
        perror("Failed to create device5");
        return -1;
        }
    else
        {
        printf("l'indicateur de device5 : %d \n",fd5);
        }








}
