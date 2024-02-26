#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include "hamming.h"
#include <linux/uaccess.h>
#include "edac_ioctl.h"
#include <linux/ioctl.h>
#include <linux/timer.h>
#include <linux/interrupt.h>

/* code matrix 2nd, 3rd, and 4th MSB define parity bits */
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

/* parity check matrix.  4 LSBs indicate which bits a parity bit represents */
const unsigned char h[PARITY_BITS] =
{
	    0x47,   /* 1 0 0 | 0 1 1 1 */
	        0x2B,   /* 0 1 0 | 1 0 1 1 */
		    0x1D    /* 0 0 1 | 1 1 0 1 */
};

/* convert syndrome (H x data) to mask that corrects data error */
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

/* table convering encoded value (with error) to original data */
/* hammingDecodeValues[code] = original data */
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

/* packed table convering encoded value (with error) to original data */
/* hammingPackedDecodeValues[code / 2] & 0x0F = original data for odd codes */
/* hammingPackedDecodeValues[code / 2] >> 4 = original data for even codes */
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

/***************************************************************************
 * *                                FUNCTIONS
 * ***************************************************************************/

/***************************************************************************
 * *   Function   : SumBitsModulo2
 * *   Description: This function performs a modulo 2 sum of the least
 * *                significant CODE_BITS bits in an unsigned char.
 * *   Parameters : bits - right justified array of CODE_BITS bits to be
 * *                       summed modulo 2.
 * *   Effects    : None
 * *   Returned   : 0 if even number of bits set to 1.
 * *                1 if odd number of bits set to 1.
 * ***************************************************************************/
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

/***************************************************************************
 * *   Function   : HammingMatrixEncode
 * *   Description: This function uses a generator matrix G (above) to
 * *                determine the Hamming code for a DATA_BITS long value.
 * *                Actually, G Transpose is used in the calculation, because
 * *                it makes the C code a little easier.
 * *                Notice that '&' is used for modulo 2 multiplication.
 * *   Parameters : data - DATA_BITS long series of data bits to encode.
 * *   Effects    : None
 * *   Returned   : Hamming code used to represent data
 * ***************************************************************************/
unsigned char HammingMatrixEncode(unsigned char data)
{
	    unsigned char i, code;
	        unsigned char rowBits;

		    code = 0;

		        for (i = 0; i < CODE_BITS; i++)
				    {
					            code <<= 1;     /* code bits are accumlated from right to left */

						            /* each component of data row times column c[i] */
						            rowBits = (gT[i] & data);       /* mod 2 multiplication of vectors */

							            /* determine modulo 2 sum of compoments */
							            if (SumBitsModulo2(rowBits))
									            {
											                code |= 1;
													        }
								        }

			    return code;
}

/***************************************************************************
 * *   Function   : HammingTableEncode
 * *   Description: This function uses a lookup table to determine the
 * *                Hamming code for a DATA_BITS long value.  The lookup table
 * *                is based on the codes generated by the matrix G (above).
 * *   Parameters : data - DATA_BITS long series of data bits to encode.
 * *   Effects    : None
 * *   Returned   : Hamming code used to represent data
 * ***************************************************************************/
unsigned char HammingTableEncode(unsigned char data)
{
	    return hammingCodes[data];
}

/***************************************************************************
 * *   Function   : HammingMatrixDecode
 * *   Description: This function uses the matrix H (above) to determine the
 * *                value encoded by a CODE_BITS long code.  H is a parity
 * *                check matrix based on the encoding matrix G.  The result
 * *                of multiplying the code by H is called the syndrome.  If
 * *                there are no errors in the code, the syndrome will be a 0
 * *                vector.  If the syndrome is not 0, it will match a column
 * *                in H.  The column it matches is likely the errored bit.
 * *                Toggle the errored bit and the resulting code is the
 * *                nearest matching correct code.
 * *   Parameters : code - CODE_BITS long series of code bits to decode.
 * *   Effects    : None
 * *   Returned   : Nearest value to encoded data
 * ***************************************************************************/
unsigned char HammingMatrixDecode(unsigned char code)
{
	    unsigned char i, syndromeVal;
	        unsigned char syndromeColBits;  /* sum of bits is bit in syndrome */

		    syndromeVal = 0;

		        for (i = 0; i < PARITY_BITS; i++)
				    {
					            syndromeVal <<= 1;

						            /* components of row h[i] times column code */
						            syndromeColBits = (h[i] & code);

							            /* determine modulo 2 sum of compoments */
							            if (SumBitsModulo2(syndromeColBits))
									            {
											                syndromeVal |= 1;
													        }
								        }

			    /* return the data corrected for error */
			    return ((code ^ syndromeMask[syndromeVal]) & (0xFF >> DATA_BITS));
}

/***************************************************************************
 * *   Function   : HammingTableDecode
 * *   Description: This function uses a lookup table to determine the value
 * *                encoded by a CODE_BITS long code.  It assumes that the
 * *                code is based on the codes generated by the matrix G
 * *                (above), but errors are allowed.  The value returned is
 * *                the value with the code nearest (with the least error) to
 * *                the code passed as a parameter.
 * *   Parameters : code - CODE_BITS long series of code bits to decode.
 * *   Effects    : None
 * *   Returned   : Nearest value to encoded data
 * ***************************************************************************/
unsigned char HammingTableDecode(unsigned char code)
{
	    return hammingDecodeValues[code];
}

/***************************************************************************
 * *   Function   : HammingPackedTableDecode
 * *   Description: This function uses a packed lookup table to determine the
 * *                value encoded by a CODE_BITS long code.  It assumes that
 * *                the code is based on the codes generated by the matrix G
 * *                (above), but errors are allowed.  The value returned is
 * *                the value with the code nearest (with the least error) to
 * *                the code passed as a parameter.
 * *                NOTE: The table packing will only work if the size of a
 * *                      table entry is larger than 2 * DATA_BITS.
 * *   Parameters : code - CODE_BITS long series of code bits to decode.
 * *   Effects    : None
 * *   Returned   : Nearest value to encoded data
 * ***************************************************************************/
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


#define DEFAULT_MEMORY_SIZE 1024
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oumaima EL MOUMEN");
MODULE_DESCRIPTION("Simple module for displaying messages in the system log");
MODULE_VERSION("0.2");

struct cdev my_cdev;

struct edac_data {
    unsigned char *memory;
    size_t memorysize;
    struct mutex mutex;
    bool oper_effec;
    struct timer_list scrub_timer; 
    struct tasklet_struct scrub_tasklet; 
    unsigned int scrub_period;     
};

static void scrub_data(struct edac_data *edac)
{
    mutex_lock(&edac->mutex);
    size_t i;
    for (i = 0; i < edac->memorysize; ++i)
    {
        unsigned int *data_block = (unsigned int *)(edac->memory + i);
        unsigned int read_data = *data_block;
        unsigned int corrected_data = HammingMatrixDecode(read_data);

        *data_block = HammingMatrixEncode(corrected_data);
    }

    mutex_unlock(&edac->mutex);
    
}


static void scrub_timer_callback(struct timer_list *t) {
    struct edac_data *data = from_timer(data, t, scrub_timer);
    tasklet_schedule(&data->scrub_tasklet);
    mod_timer(&data->scrub_timer, jiffies + data->scrub_period);
}

static void scrub_tasklet_handler(unsigned long data) {
    struct edac_data *edac = (struct edac_data *)data;
    scrub_data(edac);
}




int mycreat(struct inode *inode, struct file *filp) {
    struct edac_data *data;
    data = kmalloc(sizeof(struct edac_data), GFP_KERNEL);
    if (!data) {
        return -ENOMEM;
    }

    data->memorysize = 1024;
    data->memory = kmalloc(data->memorysize, GFP_KERNEL);
    if (!data->memory) {
        kfree(data);
        return -ENOMEM;
    }

    mutex_init(&data->mutex);
    

    filp->private_data = data;

    return 0;
}

int myopen(struct inode *inode, struct file *filp) {
    struct file *new_filp;
    struct edac_data *data;

    // Check if the flags parameter is valid
    if ((filp->f_flags & O_ACCMODE) != O_RDWR && (filp->f_flags & O_ACCMODE) != O_RDONLY && (filp->f_flags & O_ACCMODE) != O_WRONLY) {
        printk(KERN_ERR "Invalid flags parameter\n");
        return -EINVAL;
    }

    // Try to open the file in read/write mode
    new_filp = filp_open(filp->f_path.dentry->d_name.name, O_RDWR, filp->f_inode->i_mode);

    // Check if there is an error opening the file
    if (IS_ERR(new_filp)) {
        // Send error if filp_open fails
        printk(KERN_ERR "Error opening EDAC memory with filp_open\n");
        return PTR_ERR(new_filp);
    }

    // Check if EDAC is already initialized
    if (filp->private_data) {
        printk(KERN_INFO "EDAC memory already initialized\n");
        filp_close(new_filp, NULL);
        return 0;
    }

    // Check if the O_CREAT flag is set
    if ((filp->f_flags & O_CREAT) == 0) {
        printk(KERN_ERR "EDAC memory not initialized and O_CREAT flag not set\n");
        filp_close(new_filp, NULL);
        return -EINVAL;
    }


    // Initialize the edac_data structure
    data = kmalloc(sizeof(struct edac_data), GFP_KERNEL);
    if (!data) {
        printk(KERN_ERR "Error allocating memory for edac_data\n");
        filp_close(new_filp, NULL);
        return -ENOMEM;
    }

    data->memory = kmalloc(DEFAULT_MEMORY_SIZE / 4, GFP_KERNEL);
    if (!data->memory) {
        printk(KERN_ERR "Error allocating memory for data->memory\n");
        kfree(data);
        filp_close(new_filp, NULL);
        return -ENOMEM;
    }

    data->memorysize = DEFAULT_MEMORY_SIZE / 8;
    data->oper_effec = false;

    mutex_init(&data->mutex);

    // Attach the edac_data structure to filp->private_data
    filp->private_data = data;

    // Print a message (you can replace this with your own logic)
    printk(KERN_INFO "EDAC memory initialized\n");
    
    timer_setup(&data->scrub_timer, scrub_timer_callback, (unsigned long)data);
    
    data->scrub_period = 2000;  
    mod_timer(&data->scrub_timer, jiffies + data->scrub_period);

    
    tasklet_init(&data->scrub_tasklet, scrub_tasklet_handler, (unsigned long)data);

    filp->private_data = data;
    
    
    
    
    
    // Close the file
    filp_close(new_filp, NULL);
    
    

    return 0;
}


/*int myopen(struct inode *inode, struct file *filp) {
    struct file *new_filp;
    struct edac_data *data;
    
    // Vérifier si le pointeur filp ou le chemin du fichier dans f_path.dentry est invalide
    if (!filp || !filp->f_path.dentry) {
        printk(KERN_ERR "Invalid file or file path \n");
        return -EINVAL;
    }

    // Essayer d'ouvrir le fichier en lecture/écriture
    new_filp = filp_open(filp->f_path.dentry->d_name.name, O_RDWR, filp->f_inode->i_mode);
    
    
    // Vérifier s'il y a une erreur lors de l'ouverture
    if (IS_ERR(new_filp)) {
        // S'il y a une erreur, essayer de créer le fichier
        new_filp = filp_open(filp->f_path.dentry->d_name.name, O_RDWR | O_CREAT | O_EXCL, filp->f_inode->i_mode);

        // Vérifier à nouveau s'il y a une erreur
        if (IS_ERR(new_filp)) {
            printk(KERN_ERR "Error creating or opening EDAC mem with filp_open\n");
            return PTR_ERR(new_filp); // La fonction correcte est PTR_ERR, pas PIR_ERR
        }

        // Initialiser la structure edac_data
       
        data = kmalloc(sizeof(struct edac_data), GFP_KERNEL);
        if (!data) {
            printk(KERN_ERR "Error allocating memory for edac_data\n");
            filp_close(new_filp, NULL);
            return -ENOMEM;
        }

        data->memory = kmalloc(DEFAULT_MEMORY_SIZE/4, GFP_KERNEL);
        if (!data->memory) {
            printk(KERN_ERR "Error allocating memory for data->memory\n");
            kfree(data);
            filp_close(new_filp, NULL);
            return -ENOMEM;
        }
    data->memorysize = DEFAULT_MEMORY_SIZE/8;
    data->oper_effec = false;
        
        mutex_init(&data->mutex);
        

        // Attach the edac_data structure to filp->private_data
        filp->private_data = data;
    }
    if( filp->private_data == NULL){
    data = kmalloc(sizeof(struct edac_data), GFP_KERNEL);
        if (!data) {
            printk(KERN_ERR "Error allocating memory for edac_data\n");
            filp_close(new_filp, NULL);
            return -ENOMEM;
        }

        data->memory = kmalloc(DEFAULT_MEMORY_SIZE/4, GFP_KERNEL);
        if (!data->memory) {
            printk(KERN_ERR "Error allocating memory for data->memory\n");
            kfree(data);
            filp_close(new_filp, NULL);
            return -ENOMEM;
        }
        data->memory = kmalloc(DEFAULT_MEMORY_SIZE/4, GFP_KERNEL);
         if (!data->memory) {
            printk(KERN_ERR "Error allocating memory for edac_data\n");
            filp_close(new_filp, NULL);
            return -ENOMEM;
        }

        data->memorysize = DEFAULT_MEMORY_SIZE/8;
        data->oper_effec = false;
        mutex_init(&data->mutex);
        filp->private_data = data;
       }
    printk(KERN_ERR "oumaima matkhafich");

    // Fermer le fichier
    filp_close(new_filp, NULL);

    return 0;
}*/



ssize_t myread(struct file *filp, char __user *buff, size_t count, loff_t *f_pos) {
    struct edac_data *data = filp->private_data;
    data->oper_effec = true;
    unsigned char decodedDataNext4Bits;
    unsigned char decodedDataFirst4Bits;
    unsigned char corruptedDataNext4Bits;
    unsigned char corruptedDataFirst4Bits;
    unsigned char encodedDataFirst4Bits;
    unsigned char encodedDataNext4Bits;
    unsigned char decodeinter;
    loff_t pos;

    if (count < 0) {
        return -EINVAL;
    }

    mutex_lock(&data->mutex);
    pos = *f_pos;

    if ((pos + count) > data->memorysize) {
        count = data->memorysize - pos;
    }

    int i;
    for (i = 0; i < count; i++) {
        corruptedDataNext4Bits = *(data->memory + (pos + i) * 2);
        decodedDataNext4Bits = HammingMatrixDecode(corruptedDataNext4Bits);

       

        corruptedDataFirst4Bits = *(data->memory + (pos + i) * 2 + 1);
        decodedDataFirst4Bits = HammingMatrixDecode(corruptedDataFirst4Bits);

    
        decodeinter=(decodedDataNext4Bits << 4) | decodedDataFirst4Bits;
        copy_to_user(buff + i , &decodeinter, 1) ;
        

        
        encodedDataNext4Bits = HammingMatrixEncode(decodedDataNext4Bits);
        *(data->memory + (pos + i) * 2)= encodedDataNext4Bits;
        
        encodedDataFirst4Bits = HammingMatrixEncode(decodedDataFirst4Bits);
        *(data->memory + (pos + i) * 2 + 1)=encodedDataFirst4Bits;
    }

    //*f_pos += count;
    mutex_unlock(&data->mutex);

    return count;
}


ssize_t mywrite(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos) {
    struct edac_data *data = filp->private_data;
    data->oper_effec = true;
    unsigned char encodedDataFirst4Bits;
    unsigned char encodedDataNext4Bits;
    unsigned char encodedinter;
    if (count < 0) {
        return -EINVAL;
    }

    mutex_lock(&(data->mutex));
    int pos = *f_pos;
     
    if ((pos + count) > data->memorysize) {
        count = data->memorysize - pos;
    }
    int i;
    for (i = 0; i < count; i++) {
        copy_from_user(&encodedinter, buff+i, 1);
        encodedDataNext4Bits = HammingMatrixEncode(encodedinter >> 4);
        *(data->memory + (pos + i) * 2)= encodedDataNext4Bits;
        
        encodedDataFirst4Bits = HammingMatrixEncode(encodedinter & 0x0F);
        *(data->memory + (pos + i) * 2 + 1)=encodedDataFirst4Bits;
     
    }
    
    //*f_pos += count;
    mutex_unlock(&(data->mutex));
    printk(KERN_ERR "count = %ld", count);
    return count;
}
loff_t mylseek(struct file *filp, loff_t offset, int whence) {
    struct edac_data *data = filp->private_data;
    loff_t new_offset;
    int err = 0;

    mutex_lock(&(data->mutex));

    switch (whence) {
    case SEEK_SET:
        new_offset = offset;
        break;

    case SEEK_CUR:
        new_offset = filp->f_pos + offset;
        break;

    case SEEK_END:
        new_offset = data->memorysize - offset;
        break;

    default:
        err = -EINVAL;
        goto out;
    }

    if (new_offset < 0 || new_offset > data->memorysize) {
        err = -EINVAL;
        goto out;
    }

    filp->f_pos = new_offset;

out:
    mutex_unlock(&(data->mutex));

    if (err) {
        
        return (loff_t)-1;
    }

    return new_offset;
}
long myioctl(struct file *filp, unsigned int cmd, unsigned long arg) {
    struct edac_data *data = filp->private_data;
    loff_t f_pos = filp->f_pos;
    //printk( "pos : %lld \n", f_pos);
    printk( "cmd : %x \n", cmd);
    //printk( "memory : %hhn \n", data->memory);
    switch (cmd) {
        case EDAC_SETMEMORYSIZE :
            printk( "entrée \n");
            if (data->oper_effec == true) {
                printk( "test \n");
                return -EBUSY;
            }
            if (arg <= 0) {
                return -EINVAL;
            }
            mutex_lock(&data->mutex);
            char *new_memory;
            new_memory = krealloc(data->memory, arg*2, GFP_KERNEL);
            
            if (!new_memory) {
                printk( "test 2 \n");
                mutex_unlock(&data->mutex);
                return -ENOMEM;
            }   
            printk( "memory2 : %hhn \n", new_memory);
            data->memory = new_memory;
            if (!data->memory) {
                printk( "test 3 \n");
                mutex_unlock(&data->mutex);
                return -ENOMEM;
            }
            data->memorysize = arg;
            printk( "size2 : %ld \n", data->memorysize);
            mutex_unlock(&data->mutex);
            return data->memorysize;

        case EDAC_GETMEMORYSIZE :
        printk( "entrée 2\n");
    if (arg != NULL) {
            printk( "test 2 \n");
        if (put_user(data->memorysize, (size_t __user *)arg)) {
            return -EFAULT;
        }
    }   
    printk( "size fct get : %ld \n", data->memorysize);
    return data->memorysize;

        case EDAC_FAULT:
            if (arg < 0 || arg >= data->memorysize * 8) {
                return -EINVAL;
            }
            mutex_lock(&data->mutex);
            data->memory[arg / 8] ^= 1 << (arg % 8);
            mutex_unlock(&data->mutex);
            break;
        case EDAC_PERIOD:
            data->scrub_period = arg;  // arg is the new period
            if (data->scrub_period <= 0) {
                
                return -1;
                }
	      

            // Réarmer le timer avec la nouvelle période
            mod_timer(&data->scrub_timer, jiffies + msecs_to_jiffies(data->scrub_period));
            return 0;
            break;
        default:
            return -ENOTTY;
    }

    return 0;
}


int myclose(struct inode *inode, struct file *filp) {
    struct edac_data *data = filp->private_data;

    if (!data) {
        return -EBADF;
    }
    
    del_timer_sync(&data->scrub_timer);
    tasklet_kill(&data->scrub_tasklet);

    kfree(data->memory);
    kfree(data);

    filp_close(filp, NULL);  

    return 0;
} 

static struct file_operations my_fops = {
    .open = myopen,
    
    .read = myread,
    .write = mywrite,
    .llseek = mylseek,
    /*.release = myclose,*/
    .unlocked_ioctl = myioctl,
    
};

dev_t dev = MKDEV(239, 0);

static int __init mmodule_init(void) {
    int status;
    printk("EDAC_GETMEMORYSIZE: %lx\n", EDAC_GETMEMORYSIZE);
printk("EDAC_SETMEMORYSIZE: %lx\n", EDAC_SETMEMORYSIZE);
printk("EDAC_FAULT: %lx\n", EDAC_FAULT);
    if (register_chrdev_region(dev, 4, "mmodule") != 0)
        printk(KERN_INFO "Problem!\n");

    printk(KERN_INFO "Loaded!\n");
     
    cdev_init(&my_cdev, &my_fops);
    printk(KERN_INFO "%d, %d \n ", MAJOR(dev), MINOR(dev));

    status = cdev_add(&my_cdev, dev, 4);
    if (status == 0)
        printk(KERN_INFO "cDev Passed \n");
    else
        printk(KERN_INFO "Failed %d", status);
    
    return 0;
}

static void __exit mmodule_exit(void) {
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev, 4);
    printk(KERN_INFO "Exiting!\n");
}

module_init(mmodule_init);
module_exit(mmodule_exit);
