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
    if (ioctl(fd1, EDAC_SETMEMORYSIZE, &newMemorySize)) {
        perror("EDAC_SETMEMORYSIZE failed");
        close(fd);
        return -1;
    }
    int memorySize;
    memorySize = ioctl(fd, EDAC_GETMEMORYSIZE)) 
    if(memorySize == -1){
        perror("EDAC_GETMEMORYSIZE failed");
  
        return -1;
    }

    printf("Memory size: %d\n", memorySize);
    
    else
        {
        printf("l'indicateur de device1 : %d \n",fd1);
        }
    
    int new_offset = lseek (fd1, 10, SEEK_SET); 
    if((new_offest)==-1)
        {
        perror("erreur changementd'offset");
        //return -1;
        }
    else
        {
        printf("nouveau offset : %d \n",new_offset);
        }
    new_offset = lseek (fd1, -5, SEEK_SET); 
    if((new_offest)==-1)
        {
        perror("erreur changementd'offset");
        //return -1;
        }
    else
        {
        printf("nouveau offset : %d \n",new_offset);
        }
    new_offset = lseek (fd2, 0, SEEK_SET); 
    if((new_offest)==-1)
        {
        perror("erreur changementd'offset");
        //return -1;
        }
    else
        {
        printf("nouveau offset : %d \n",new_offset);
        }
    new_offset = lseek (fd1, 5, SEEK_SET); 
    if((new_offest)==-1)
        {
        perror("erreur changementd'offset");
        //return -1;
        }
    else
        {
        printf("nouveau offset : %d \n",new_offset);
        }
    new_offset = lseek (fd1, 2, SEEK_CUR); 
    if((new_offest)==-1)
        {
        perror("erreur changementd'offset");
        //return -1;
        }
    else
        {
        printf("nouveau offset : %d \n",new_offset);
        }
    new_offset = lseek (fd1, 2, SEEK_END); 
    if((new_offest)==-1)
        {
        perror("erreur changementd'offset");
        //return -1;
        }
    else
        {
        printf("nouveau offset : %d \n",new_offset);
        }      
    new_offset = lseek (fd1, 5, SEEK_CUR); 
    if((new_offest)==-1)
        {
        perror("erreur changementd'offset");
        //return -1;
        }
    else
        {
        printf("nouveau offset : %d \n",new_offset);


return 0;
}
