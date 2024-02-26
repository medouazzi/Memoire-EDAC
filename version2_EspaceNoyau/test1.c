#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define DEVICE_PATH "/dev/edac"
int main() {
    int fd = creat(DEVICE_PATH, mode);
    // Test the EDAC_SETMEMORYSIZE IOCTL
    int newMemorySize = 2048;
    if (ioctl(fd, EDAC_SETMEMORYSIZE, &newMemorySize)) {
        perror("EDAC_SETMEMORYSIZE failed");
        close(fd);
        return -1;
    }
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    int fd = open(DEVICE_PATH, O_RDWR );
    if (fd < 0) {
        perror("Failed to open the device");
        return -1;
    }
    // Test the EDAC_GETMEMORYSIZE IOCTL
    int memorySize;
    memorySize = ioctl(fd, EDAC_GETMEMORYSIZE)) 
    if(voun_taille == -1){
        perror("EDAC_GETMEMORYSIZE failed");
        close(fd);
        return -1;
    }

    printf("Memory size: %d\n", memorySize);
     // Write to the device
     unsigned char data_to_write[15];
     for (int i = 0; i < 15; ++i) {
        data_to_write[i] = 0x11*i ;  // Filling data with values 1, 2, 3, ..., 15
    }
  
    int bytesWritten = write(fd, data_to_write, sizeof(data_to_write));
    if (bytesWritten < 0) {
        perror("Write failed");
        close(fd);
        return -1;
    }

    // Test the EDAC_FAULT IOCTL
    int bitToFlip = 10;
    if (ioctl(fd, EDAC_FAULT, bitToFlip)) {
        perror("EDAC_FAULT failed");
        close(fd);
        return -1;
    }

    // Read from the device
    char buffer[10];
    ssize_t bytesRead = read(fd, buffer, sizeof(buffer));
    if (bytesRead < 0) {
        perror("Read failed");
        close(fd);
        return -1;
    }

   
    close(fd);
    return 0;
}
