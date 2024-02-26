#define DEVICE_PATH "/dev/edac"
int main() {
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    int fd = creat(DEVICE_PATH, mode);
    // Test the EDAC_SETMEMORYSIZE IOCTL
    int newMemorySize = 128;
    if (ioctl(fd, EDAC_SETMEMORYSIZE, &newMemorySize)) {
        perror("EDAC_SETMEMORYSIZE failed");
        close(fd);
        return -1;
    }
    
    int fd = open(DEVICE_PATH, O_RDWR );
    if (fd < 0) {
        perror("Failed to open the device");
        return -1;
    }
    // Test the EDAC_GETMEMORYSIZE IOCTL
    int memorySize;
    memorySize = ioctl(fd, EDAC_GETMEMORYSIZE)) 
    if(memorySize == -1){
        perror("EDAC_GETMEMORYSIZE failed");
  
        return -1;
    }

    printf("Memory size: %d\n", memorySize);
     // Write to the device
     unsigned char data_to_write[18];
     for (int i = 0; i < 18; ++i) {
        data_to_write[i] = 0x11*i ;  // Filling data with values 1, 2, 3, ..., 15
    }
  
    int bytesWritten = write(fd, data_to_write, strlen(message));
    if (bytesWritten < 0) {
        perror("Write failed");
        
        return -1;
    }

    // Test the EDAC_FAULT IOCTL
    int bitToFlip = 10;
    if (ioctl(fd, EDAC_FAULT, bitToFlip)) {
        perror("EDAC_FAULT failed");
       
        return -1;
    }

    // Read from the device
    char buffer[18];
    ssize_t bytesRead = read(fd, buffer, sizeof(buffer));
    if (bytesRead < 0) {
        perror("Read failed");
        return -1;
    }

   
    return 0;
}
