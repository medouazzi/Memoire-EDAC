#define     DEVICE1_PATH "/dev/edac1"

int main() {
    int fd1,fd2;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH; // Read and write permissions for owner and group, read permission for others

// CREATIONS DES DEVICES*
    
    if((fd1 = open(DEVICE1_PATH,O_WRONLY | O_CREAT, mode))==-1)
        {
        perror("erreur ouvertuere device1");
        //return -1;
        }
    int newMemorySize = 128;
    if (ioctl(fd2, EDAC_SETMEMORYSIZE, &newMemorySize)) {
        perror("EDAC_SETMEMORYSIZE failed");
        
        return -1;
    }
    if (ioctl(fd1, EDAC_SETMEMORYSIZE, &newMemorySize)) {
        perror("EDAC_SETMEMORYSIZE failed");
        
        return -1;
    }
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
  
    int bytesWritten = write(fd1, data_to_write, sizeof(data_to_write));
    if (bytesWritten < 0) {
        perror("Write failed");
        
        return -1;
    }
    newMemorySize = 30;
    if (ioctl(fd1, EDAC_SETMEMORYSIZE, &newMemorySize)) {
        perror("EDAC_SETMEMORYSIZE failed");
        
        return -1;
    }

return 0;
}
