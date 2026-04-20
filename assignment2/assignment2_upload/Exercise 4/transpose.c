#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

/* Default matrix size if no CLI argument is given. */
#define N_DEFAULT 2000
#define BLOCK_DEFAULT 32

static double* a = NULL;
static double* b = NULL;

static inline int min_int(int x, int y) { return (x < y) ? x : y; }

#define A(n, i, j) a[(size_t)(i) * (n) + (j)]
#define B(n, i, j) b[(size_t)(i) * (n) + (j)]

// timer
double mysecond(){
  struct timeval tp;
  struct timezone tzp;
  int i;

  i = gettimeofday(&tp,&tzp);
  return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
}

// basic transposition without code transformations
void transposeBase(int n){
  int i, j;

  for (i = 0; i < n; ++i)
    for (j = 0; j < n; ++j)
      B(n, i, j) = A(n, j, i);

}

/* Cache-friendly blocked transpose. */
void transposeBlocked(int n, int block_size) {
  for (int ii = 0; ii < n; ii += block_size) {
    for (int jj = 0; jj < n; jj += block_size) {
      const int i_max = min_int(ii + block_size, n);
      const int j_max = min_int(jj + block_size, n);
      for (int i = ii; i < i_max; ++i) {
        for (int j = jj; j < j_max; ++j) {
          B(n, i, j) = A(n, j, i);
        }
      }
    }
  }
}


void initialize_matrices(int n){
  int i, j ;
 
  for (i = 0 ; i < n ; i++) {
    for (j = 0 ; j < n ; j++) {
      A(n, i, j) = (double) rand() / RAND_MAX;
      B(n, i, j) = (double) rand() / RAND_MAX;
    }
  }


}

int main( int argc, char *argv[] )
{
    
    int maxTest=10;
    int i, j, tests, n, block_size;
    int k, rept=10;
    double tStart, tEnd;
    double tLoop, t, rate;
    int use_blocked = 0;

    n = (argc > 1) ? atoi(argv[1]) : N_DEFAULT;
    if (n <= 0) n = N_DEFAULT;

    if (argc > 2 && strcmp(argv[2], "blocked") == 0) {
      use_blocked = 1;
    }

    block_size = (argc > 3) ? atoi(argv[3]) : BLOCK_DEFAULT;
    if (block_size <= 0) block_size = BLOCK_DEFAULT;

    a = (double*)malloc((size_t)n * n * sizeof(double));
    b = (double*)malloc((size_t)n * n * sizeof(double));
    if (!a || !b) {
      fprintf(stderr, "Allocation failed for n=%d\n", n);
      free(a);
      free(b);
      return 1;
    }

    
    printf( "Transpose %d x %d\n", n, n );
    printf( "Mode: %s\n", use_blocked ? "blocked" : "base" );
    if (use_blocked) printf("Block size: %d\n", block_size);

    initialize_matrices(n);
    
    // Start tests
    tLoop = 1.0e10; // take a large number
    for (tests=0; tests<maxTest; tests++) {
	     
       tStart = mysecond();
	     
       for (k=0; k<rept; k++) {
          if (use_blocked) {
            transposeBlocked(n, block_size);
          } else {
            transposeBase(n);
          }
	     }
       
       tEnd = mysecond();
	     t =  tEnd - tStart;
	     t = t / rept;
       // record the best time for all the different tests
	     if (t < tLoop) tLoop = t;
    }
    rate = 8*((double)n * n) / tLoop;
    printf( "BaseTime = %.2e s, Base Rate = %.2e MB/s\n", tLoop, rate*1.0e-6);

    // reuse b to avoid smart compiler issues
    for (i = 0 ; i < n ; i++) 
      for (j = 0 ; j < n ; j++) 
        B(n, i, j)++;
    
    free(a);
    free(b);

    return 0;
}
