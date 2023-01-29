#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define bitblock unsigned int
#define BLKSIZ (sizeof(bitblock)*8)
#define LOOKUP 65536
unsigned char SetBitTable[LOOKUP] = {0};

// function declarations
void fillSetBitTable(unsigned char table[], int n);
int AndCount_bit(bitblock *x, bitblock *y, bitblock *z, int n_blocks);
int AndCount_int(int *x, int *y, int *z, int size);
int get_n_blocks(int size);
int pack_bitblock(int *x, bitblock *bx, int size, int n_bitblock);
void bitblock_to_bin(bitblock x, char *bin);
void print_a(int *a, int size);
void print_b(bitblock *b, int n_blocks);
void test_bit_int(int size, int n_trials, int verbose);

double (*timenow)(void);
#ifdef _WIN32
double timenow_windows(void){
    return (double) clock()/CLOCKS_PER_SEC;
}
#else
double timenow_linux(void){
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return now.tv_sec + now.tv_nsec*1e-9;
}
#endif

void fillSetBitTable(unsigned char table[], int n){
    for(int i = 0; i < n; i++){
        table[i] = (i & 1) + table[i / 2];
    }
}

void print_a(int *a, int size){
    for(int i = 0; i < size; i++){
        printf("%d ", a[i]);
        if(i % BLKSIZ == BLKSIZ - 1) printf("\n");
    }
    puts("");
}

void print_b(bitblock *b, int n_blocks){
    char buf[BLKSIZ+1];
    for(int i = 0; i < n_blocks; i++){
        bitblock_to_bin(b[i], buf);
        printf("%s\n", buf);
    }
}

void bitblock_to_bin(bitblock x, char *bin){
    for(int i = 0; i < BLKSIZ; i++){
        bin[31-i] = x & 1 << i ? '1' : '0';
    }
    bin[BLKSIZ] = '\0';
}

int pack_bitblock(int *x, bitblock *bx, int size, int n_bitblock){
    // check size
    if(n_bitblock < get_n_blocks(size)){
        fprintf(stderr, "n_blocks %d is too small for size %d\n", n_bitblock, size);
        return EXIT_FAILURE;
    }
    int block_num, bit_num;
    for(int i = 0; i < size; i++){
        block_num = i / BLKSIZ;
        bit_num = i % BLKSIZ;
        if(bit_num == 0) memset(bx + block_num, 0, sizeof(bitblock));
        if(x[i] == 1){
            bx[block_num] |= 1 << (BLKSIZ - bit_num - 1);
        }
    }
    return EXIT_SUCCESS;
}

int get_n_blocks(int size){
    return (size + BLKSIZ - 1) / BLKSIZ;
}

int AndCount_int(int *x, int *y, int *z, int size){
    int count = 0;
    for(int i = 0; i < size; i++){
        if(x[i] == 1 && y[i] == 1){
            z[i] = 1;
            count++; 
        } else {
            z[i] = 0;
        }
    }
    return count;
}

int AndCount_bit(bitblock *x, bitblock *y, bitblock *z, int n_blocks){
    int count = 0;
    for(int i = 0; i < n_blocks; i++){
        z[i] = x[i] & y[i];
        count += (int) (SetBitTable[z[i] & 0xffff] + SetBitTable[(z[i] >> 16) & 0xffff]);
    }
    return count;
}

void test_bit_int(int size, int n_trials, int verbose){
    int n_blocks = get_n_blocks(size);
    // create data
    int *a1 = malloc(size*sizeof(int));
    int *a2 = malloc(size*sizeof(int));
    bitblock *b1 = malloc(n_blocks*sizeof(bitblock));
    bitblock *b2 = malloc(n_blocks*sizeof(bitblock));
    if(verbose) printf("size = %d, n_blocks = %d\n", size, n_blocks);

    double avg_time_int = 0;
    double avg_time_bit = 0;
    for(int k = 0; k < n_trials; k++){
        if(verbose) printf("Trial %d: ", k);
        // random initialize values
        for(int i = 0; i < size; i++){
            a1[i] = rand() % 2;
            a2[i] = rand() % 2;
        }
        // bitblock packing is a one-time task in practice
        pack_bitblock(a1, b1, size, n_blocks);
        pack_bitblock(a2, b2, size, n_blocks);
        
        int count_int, count_bit;
        double begtime, endtime;
        double time_int, time_bit;
        begtime = timenow();
        count_int = AndCount_int(a1,a2,a1,size);
        endtime = timenow();
        time_int = endtime - begtime;
        if(verbose) printf("Count int: %d, time: %0.5f | ", count_int, time_int);
        avg_time_int += time_int;

        begtime = timenow();
        count_bit = AndCount_bit(b1,b2,b1,n_blocks);
        endtime = timenow();
        time_bit = endtime - begtime;
        if(verbose) printf("Count bit: %d, time: %0.5f\n", count_bit, time_bit);
        avg_time_bit += time_bit;

        //print_a(a1, size);
        //print_b(b1, n_blocks);
    }
    avg_time_int /= n_trials;
    avg_time_bit /= n_trials;
    if(verbose) puts("Summary (size avg_time_int avg_time_bit):");
    printf("%d %0.5f %0.5f\n", size, avg_time_int, avg_time_bit);

    // clean up
    free(a1);
    free(a2);
    free(b1);
    free(b2);
}

void main(int argc, char *argv[]){
#ifdef _WIN32
    timenow = timenow_windows;
#else
    timenow = timenow_linux;
#endif
    if(argc < 2){
        printf("Usage: %s <array_size> [n_trials] [verbose]\n", argv[0]);
        exit(0);
    }
    fillSetBitTable(SetBitTable, LOOKUP);
    int size = atoi(argv[1]);
    int n_trials = 1;
    int verbose = 1;
    if(argc >=3) n_trials = atoi(argv[2]);
    if(argc >= 4) verbose = atoi(argv[3]);
    test_bit_int(size, n_trials, verbose);
}