#define     DEVICE1_PATH "/dev/edac1"

int main() {
    int fd1,fd2;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH; // Read and write permissions for owner and group, read permission for others

// CREATIONS DES DEVICES
    if((fd1 = open(DEVICE1_PATH,O_WRONLY | O_CREAT, mode))==-1)
        {
        perror("erreur ouvertuere device1");
        //return -1;
        }

    else
        {
        printf("l'indicateur de device1 : %d \n",fd1);
        }
     unsigned char data_to_write[15];
     for (int i = 0; i < 15; ++i) {
        data_to_write[i] = 0x11*i ;  // Filling data with values 1, 2, 3, ..., 15
    }
    int bytesWritten1 = write(fd1, data_to_write, sizeof(data_to_write));
    if (bytesWritten < 0) {
        perror("Write failed");
        //close(fd);
       // return -1;
    }
    ssize_t bytesRead = read(fd1, buffer, sizeof(buffer));
    if (bytesRead < 0) {
        perror("Read failed");
        return -1;
    }
    bytesRead = read(fd1, buffer, -5);
    if (bytesRead < 0) {
        perror("Read failed");
        return -1;
    }
    bytesRead = read(fd2, buffer, sizeof(buffer));
    if (bytesRead < 0) {
        perror("Read failed");
        return -1;
    }
   
    return 0;



}
