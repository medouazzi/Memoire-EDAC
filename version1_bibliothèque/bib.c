#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include "hamming.h"
#include  <string.h>
#define SHM_KEY 1234  

#include "bib.h"

int pthread_edac_init(struct pthread_edac_t* edac, int memorysize) {
    if (edac == NULL || memorysize <= 0) {
        errno = EINVAL;
        return -1;
    }

    int mutex_init_result = pthread_mutex_init(&(edac->mutex), NULL);
    if (mutex_init_result != 0) {
        return mutex_init_result;  
    }

 edac->shm_id = shmget((int)edac,memorysize , IPC_CREAT | 0666);
        if (edac->shm_id == -1) {
	      perror("Erreur lors de la création/accès au segment de mémoire partagé");
			        exit(1);
				    }


    edac->memory = (unsigned char *)shmat(edac->shm_id, NULL, 0);
        if ((intptr_t)edac->memory == -1) {
           perror("Erreur lors de l'attachement du segment de mémoire partagé");
			        exit(1);
				    }



    if (edac->memory == NULL) {
        errno = ENOMEM;
        pthread_mutex_destroy(&edac->mutex);
        return -1;
    }
 
    
    
    edac->memorysize = memorysize;
    return 0;
}




int pthread_edac_destroy(struct pthread_edac_t* edac) {
    if (edac == NULL) {
        errno = EINVAL;
        return -1;
    }
    shmdt(edac->memory);
        shmctl(edac->shm_id, IPC_RMID, NULL);
    int mutex_destroy_result = pthread_mutex_destroy(&edac->mutex);
    if (mutex_destroy_result != 0) {
        return mutex_destroy_result;  
    }
    
    
    return 0;
}


int pthread_edac_write(struct pthread_edac_t* edac, unsigned char* data, int size, int offset) {
    if (edac == NULL || data == NULL || size <= 0 || offset < 0 || offset > ((edac->memorysize)-1)) {
        errno = EINVAL;
        return -1;
    }
    int mutex_lock_result = pthread_mutex_lock(&(edac->mutex));
    if (mutex_lock_result != 0) {
        return mutex_lock_result;  
    }
    if ((offset + size)*2 > edac->memorysize) {
        size = (edac->memorysize)/2 - offset; 
    }
     unsigned char encodedDataFirst4Bits ;
    unsigned char encodedDataNext4Bits;
    printf("Thread ID: %lu,: \n write data :", (unsigned long)pthread_self());
    for(int i=0 ; i<size ;i++){

            encodedDataNext4Bits = HammingMatrixEncode((data[i] >> 4));
            memcpy(edac->memory + (offset + i)*2, &encodedDataNext4Bits, 1);
     
            encodedDataFirst4Bits = HammingMatrixEncode(data[i]);
            memcpy(edac->memory + (offset + i)*2 + 1, &encodedDataFirst4Bits, 1);
            printf("0x%02X \t", data[i]);
       
}
    
    printf( "\n");

    int mutex_unlock_result = pthread_mutex_unlock(&(edac->mutex));
    if (mutex_unlock_result != 0) {
        return mutex_unlock_result;  
    }
    return size;
}

    int pthread_edac_read(struct pthread_edac_t* edac, unsigned char* data, int size, int offset) {
    if (edac == NULL || data == NULL || size <= 0 || offset < 0|| offset > ((edac->memorysize)-1)) {
        errno = EINVAL;
        return -1;
    }
    int mutex_lock_result = pthread_mutex_lock(&(edac->mutex));
    if (mutex_lock_result != 0) {
        return mutex_lock_result; 
    }

     if ((offset + size)*2 > edac->memorysize) {
        size = (edac->memorysize)/2 - offset; 
    }
    unsigned char decodedDataNext4Bits ;
    unsigned char decodedDataFirst4Bits;
    unsigned char corruptedDataNext4Bits ;
    unsigned char corruptedDataFirst4Bits;
    unsigned char encodedDataFirst4Bits ;
    unsigned char encodedDataNext4Bits;
    printf("Thread ID: %lu, Read Data: \n ", (unsigned long)pthread_self());
    for(int i=0 ; i<size ;i++){
            memcpy(&corruptedDataNext4Bits, edac->memory + (offset+i)*2, 1);
         
            decodedDataNext4Bits = HammingMatrixDecode(corruptedDataNext4Bits);

          
        
            memcpy(&corruptedDataFirst4Bits, edac->memory + (offset+i)*2+1, 1);
        
            decodedDataFirst4Bits = HammingMatrixDecode(corruptedDataFirst4Bits);

           

            data[i] = (decodedDataNext4Bits << 4) | decodedDataFirst4Bits;
            encodedDataNext4Bits = HammingMatrixEncode((data[i] >> 4));
            memcpy(edac->memory + (offset + i)*2, &encodedDataNext4Bits, 1);
     
            encodedDataFirst4Bits = HammingMatrixEncode(data[i]);
            memcpy(edac->memory + (offset + i)*2 + 1, &encodedDataFirst4Bits, 1);
            printf("0x%02X \t", data[i]);                   
}
    
    

   
    printf("\n");


    int mutex_unlock_result = pthread_mutex_unlock(&(edac->mutex));
    if (mutex_unlock_result != 0) {
        return mutex_unlock_result; 
    }

    return size;  
}



const unsigned char g[DATA_BITS] =
{
    0x38,   /* 0 1 1 | 1 0 0 0 */
    0x54,   /* 1 0 1 | 0 1 0 0 */
    0x62,   /* 1 1 0 | 0 0 1 0 */
    0x71    /* 1 1 1 | 0 0 0 1 */
};

/* code matrix trasposed for ease of use in routines */
const unsigned char gT[CODE_BITS] =
{
    0x07,   /* 0 1 1 1 */
    0x0B,   /* 1 0 1 1 */
    0x0D,   /* 1 1 0 1 */
            /* ------- */
    0x08,   /* 1 0 0 0 */
    0x04,   /* 0 1 0 0 */
    0x02,   /* 0 0 1 0 */
    0x01    /* 0 0 0 1 */
};

/* table of Hamming codes hammingCodes[x] is the x encoded */
const unsigned char hammingCodes[DATA_VALUES] =
{
    0x00,   /* 0 */
    0x71,   /* 1 */
    0x62,   /* 2 */
    0x13,   /* 3 */
    0x54,   /* 4 */
    0x25,   /* 5 */
    0x36,   /* 6 */
    0x47,   /* 7 */
    0x38,   /* 8 */
    0x49,   /* 9 */
    0x5A,   /* A */
    0x2B,   /* B */
    0x6C,   /* C */
    0x1D,   /* D */
    0x0E,   /* E */
    0x7F    /* F */
};


const unsigned char h[PARITY_BITS] =
{
    0x47,   /* 1 0 0 | 0 1 1 1 */
    0x2B,   /* 0 1 0 | 1 0 1 1 */
    0x1D    /* 0 0 1 | 1 1 0 1 */
};


const unsigned char syndromeMask[PARITY_VALUES] =
{
    0x00,   /* syndrome = 0 0 0 */
    0x10,   /* syndrome = 0 0 1 */
    0x20,   /* syndrome = 0 1 0 */
    0x08,   /* syndrome = 0 1 1 */
    0x40,   /* syndrome = 1 0 0 */
    0x04,   /* syndrome = 1 0 1 */
    0x02,   /* syndrome = 1 1 0 */
    0x01    /* syndrome = 1 1 1 */
};


const unsigned char hammingDecodeValues[CODE_VALUES] =
{
    0x00, 0x00, 0x00, 0x03, 0x00, 0x05, 0x0E, 0x07,     /* 0x00 to 0x07 */
    0x00, 0x09, 0x0E, 0x0B, 0x0E, 0x0D, 0x0E, 0x0E,     /* 0x08 to 0x0F */
    0x00, 0x03, 0x03, 0x03, 0x04, 0x0D, 0x06, 0x03,     /* 0x10 to 0x17 */
    0x08, 0x0D, 0x0A, 0x03, 0x0D, 0x0D, 0x0E, 0x0D,     /* 0x18 to 0x1F */
    0x00, 0x05, 0x02, 0x0B, 0x05, 0x05, 0x06, 0x05,     /* 0x20 to 0x27 */
    0x08, 0x0B, 0x0B, 0x0B, 0x0C, 0x05, 0x0E, 0x0B,     /* 0x28 to 0x2F */
    0x08, 0x01, 0x06, 0x03, 0x06, 0x05, 0x06, 0x06,     /* 0x30 to 0x37 */
    0x08, 0x08, 0x08, 0x0B, 0x08, 0x0D, 0x06, 0x0F,     /* 0x38 to 0x3F */
    0x00, 0x09, 0x02, 0x07, 0x04, 0x07, 0x07, 0x07,     /* 0x40 to 0x47 */
    0x09, 0x09, 0x0A, 0x09, 0x0C, 0x09, 0x0E, 0x07,     /* 0x48 to 0x4F */
    0x04, 0x01, 0x0A, 0x03, 0x04, 0x04, 0x04, 0x07,     /* 0x50 to 0x57 */
    0x0A, 0x09, 0x0A, 0x0A, 0x04, 0x0D, 0x0A, 0x0F,     /* 0x58 to 0x5F */
    0x02, 0x01, 0x02, 0x02, 0x0C, 0x05, 0x02, 0x07,     /* 0x60 to 0x67 */
    0x0C, 0x09, 0x02, 0x0B, 0x0C, 0x0C, 0x0C, 0x0F,     /* 0x68 to 0x6F */
    0x01, 0x01, 0x02, 0x01, 0x04, 0x01, 0x06, 0x0F,     /* 0x70 to 0x77 */
    0x08, 0x01, 0x0A, 0x0F, 0x0C, 0x0F, 0x0F, 0x0F      /* 0x78 to 0x7F */
};


const unsigned char hammingPackedDecodeValues[CODE_VALUES] =
{
    0x00, 0x03, 0x05, 0xE7,     /* 0x00 to 0x07 */
    0x09, 0xEB, 0xED, 0xEE,     /* 0x08 to 0x0F */
    0x03, 0x33, 0x4D, 0x63,     /* 0x10 to 0x17 */
    0x8D, 0xA3, 0xDD, 0xED,     /* 0x18 to 0x1F */
    0x05, 0x2B, 0x55, 0x65,     /* 0x20 to 0x27 */
    0x8B, 0xBB, 0xC5, 0xEB,     /* 0x28 to 0x2F */
    0x81, 0x63, 0x65, 0x66,     /* 0x30 to 0x37 */
    0x88, 0x8B, 0x8D, 0x6F,     /* 0x38 to 0x3F */
    0x09, 0x27, 0x47, 0x77,     /* 0x40 to 0x47 */
    0x99, 0xA9, 0xC9, 0xE7,     /* 0x48 to 0x4F */
    0x41, 0xA3, 0x44, 0x47,     /* 0x50 to 0x57 */
    0xA9, 0xAA, 0x4D, 0xAF,     /* 0x58 to 0x5F */
    0x21, 0x22, 0xC5, 0x27,     /* 0x60 to 0x67 */
    0xC9, 0x2B, 0xCC, 0xCF,     /* 0x68 to 0x6F */
    0x11, 0x21, 0x41, 0x6F,     /* 0x70 to 0x77 */
    0x81, 0xAF, 0xCF, 0xFF      /* 0x78 to 0x7F */
};


unsigned char SumBitsModulo2(unsigned char bits)
{
    unsigned char sum, mask;

    sum = 0;

    for (mask = 0x01; mask < (0x01 << CODE_BITS); mask <<=1 )
    {
        if (bits & mask)
        {
            sum++;
        }
    }

    return (sum & 0x01);
}

unsigned char HammingMatrixEncode(unsigned char data)
{
    unsigned char i, code;
    unsigned char rowBits;

    code = 0;

    for (i = 0; i < CODE_BITS; i++)
    {
        code <<= 1;    

  
        rowBits = (gT[i] & data);     

     
        if (SumBitsModulo2(rowBits))
        {
            code |= 1;
        }
    }

    return code;
}


unsigned char HammingTableEncode(unsigned char data)
{
    return hammingCodes[data];
}


unsigned char HammingMatrixDecode(unsigned char code)
{
    unsigned char i, syndromeVal;
    unsigned char syndromeColBits;  

    syndromeVal = 0;

    for (i = 0; i < PARITY_BITS; i++)
    {
        syndromeVal <<= 1;

        syndromeColBits = (h[i] & code);

    
        if (SumBitsModulo2(syndromeColBits))
        {
            syndromeVal |= 1;
        }
    }

 
    return ((code ^ syndromeMask[syndromeVal]) & (0xFF >> DATA_BITS));
}


unsigned char HammingTableDecode(unsigned char code)
{
    return hammingDecodeValues[code];
}


unsigned char HammingPackedTableDecode(unsigned char code)
{
    unsigned char decoded;

    decoded = hammingPackedDecodeValues[code / 2];

    if (code % 2)
    {
        /* even value.  keep LS nibble */
        decoded &= (0xFF >> DATA_BITS);
    }
    else
    {
        /* odd value.  move MS nibble to LS half */
        decoded >>= DATA_BITS;
    }

    return decoded;
}
