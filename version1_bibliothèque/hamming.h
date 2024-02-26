#ifndef HAMMING_H
#define HAMMING_H

#define DATA_BITS 4
#define CODE_BITS 7
#define PARITY_BITS 3
#define DATA_VALUES (1 << DATA_BITS)
#define CODE_VALUES (1 << CODE_BITS)
#define PARITY_VALUES (1 << PARITY_BITS)

unsigned char HammingMatrixEncode(unsigned char data);
unsigned char HammingMatrixDecode(unsigned char code);

#endif // HAMMING_H
