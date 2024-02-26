gcc -c bib.c -o bib.o

ar rcs libmonlib.a bib.o 

sudo cp libmonlib.a /usr/local/lib

sudo cp bib.h /usr/local/include

gcc test.c -o test -lmonlib -pthread
