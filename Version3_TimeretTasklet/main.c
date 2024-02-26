#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ioctl.h>
#define DEVICE1_PATH "/dev/mmodule0"

#define EDAC_SETMEMORYSIZE 1
#define EDAC_GETMEMORYSIZE 2
#define EDAC_FAULT   3

int main() {
    int fd1;
    int offset,size;
    // Open the device with O_RDWR only (no need for mode parameter)
    if ((fd1 = open(DEVICE1_PATH, O_RDWR)) == -1) {
        perror("Device 1 not initialized");
        return EXIT_FAILURE;
    } else {
        printf("File descriptor for device 1: %d\n", fd1);
    }

    unsigned char data_to_write[15];
    for (int i = 0; i < 15; ++i) {
        data_to_write[i] = 0x11 * i;  // Filling data with values 1, 2, 3, ..., 15
    }
    offset =  lseek (fd1, 0, SEEK_CUR); 
    printf("offset: %d \n",offset);
    ioctl (fd1, EDAC_GETMEMORYSIZE,&size);
    printf("taille mémoire: %d \n",size);
    size = ioctl (fd1, EDAC_SETMEMORYSIZE, 15*8);
    printf("nouvelle taille mémoire: %d \n",size);
    size = ioctl (fd1, EDAC_GETMEMORYSIZE);
    printf("nouvelle taille mémoire: %d \n",size);
    offset =  lseek (fd1, 3, SEEK_SET); 
    // Write data to the device
    int bytesWritten1 = write(fd1,data_to_write, 15);
    printf("bytes written: %d \n",bytesWritten1);
    if (bytesWritten1 < 0) {
        perror("Write failed");
        close(fd1);
        return EXIT_FAILURE;
    }
    offset =  lseek (fd1, 0, SEEK_CUR); 
    printf("offset: %d \n",offset);
    offset =  lseek (fd1, 10, SEEK_END); 
    printf("offset: %d \n",offset);
    ioctl (fd1, EDAC_FAULT, 7);
    unsigned char buffer[15];
    // Read data from the device
    ssize_t bytesRead = read(fd1, buffer, sizeof(buffer));
    if (bytesRead < 0) {
        perror("Read failed");
        close(fd1);
        return EXIT_FAILURE;
    }

    // Display the read data
    printf("Read data: %ld \n",bytesRead);
    for (int i = 0; i < bytesRead; ++i) {
        printf("%02X ", buffer[i]);
    }
    printf("\n");
    ioctl (fd1, EDAC_FAULT, 7);
    ioctl (fd1, EDAC_FAULT, 6);
    bytesRead = read(fd1, buffer, sizeof(buffer));
    if (bytesRead < 0) {
        perror("Read failed");
        close(fd1);
        return EXIT_FAILURE;
    }

    // Display the read data
    printf("Read data: %ld \n",bytesRead);
    for (int i = 0; i < bytesRead; ++i) {
        printf("%02X ", buffer[i]);
    }
    // Close the file descriptor
    if (close(fd1) == -1) {
        perror("Error closing file descriptor");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
