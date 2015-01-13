CFLAGS=-std=c99 -g -Ofast -fopenmp 
LDFLAGS=-lcrypto
all: omp_trip 

omp_trip: tripgen.o

tripgen.o:
	$(CC) $(CFLAGS) -c tripgen.c
