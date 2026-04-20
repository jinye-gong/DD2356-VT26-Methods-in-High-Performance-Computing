#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <stdbool.h>
 
#define TRIALS 10 
#ifndef MSIZE
#define MSIZE 1000
#endif

double matrix_a[MSIZE][MSIZE];
double matrix_b[MSIZE][MSIZE];
double matrix_r[MSIZE][MSIZE];
 
double mysecond(){
  struct timeval tp;
  struct timezone tzp;
  int i;

  i = gettimeofday(&tp,&tzp);
  return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
}

void initialize_matrices()
{
  int i, j ;
 
  for (i = 0 ; i < MSIZE ; i++) {
    for (j = 0 ; j < MSIZE ; j++) {
      matrix_a[i][j] = (double) rand() / RAND_MAX;
      matrix_b[i][j] = (double) rand() / RAND_MAX;
      matrix_r[i][j] = 0.0;
    }
  }
}

void multiply_matrices_naive()
{
  int i, j, k;
 
  for (i = 0 ; i < MSIZE ; i++) {
    for (j = 0 ; j < MSIZE ; j++) {
      for (k = 0 ; k < MSIZE ; k++) {
        matrix_r[i][j] += matrix_a[i][k] * matrix_b[k][j];
      }
    }
  }
}

/* Optimized by loop reordering: i-k-j
 * - matrix_a[i][k] is reused in inner j-loop
 * - matrix_b[k][j] is accessed contiguously (row-major)
 * - matrix_r[i][j] is updated contiguously
 */
void multiply_matrices_optimized()
{
  int i, j, k;

  for (i = 0; i < MSIZE; i++) {
    for (k = 0; k < MSIZE; k++) {
      const double aik = matrix_a[i][k];
      for (j = 0; j < MSIZE; j++) {
        matrix_r[i][j] += aik * matrix_b[k][j];
      }
    }
  }
}

double average_result()
{
  int i, j ;
  double  ave = 0.0;
  for (i = 0 ; i < MSIZE ; i++) {
    for (j = 0 ; j < MSIZE ; j++) {
      ave += matrix_r[i][j]/(double)(MSIZE*MSIZE);
    }
  }
  return(ave);
}

int main(int argc, char* argv[]){
  int i;
  double t1, t2;
  bool use_optimized = false;

  if (argc > 1 && strcmp(argv[1], "opt") == 0) {
    use_optimized = true;
  }

  printf("1. Initializing Matrices \n");
  // init matrices with random numbers
  initialize_matrices();
  if (use_optimized) {
    printf("   Mode: optimized (i-k-j loop order)\n");
  } else {
    printf("   Mode: naive (i-j-k loop order)\n");
  }
  // matrix-matrix multiply: this takes most of the time
  printf("2. Matrix Multiply \n");
  if (use_optimized) {
    multiply_matrices_optimized();
  } else {
    multiply_matrices_naive();
  }
  t1 = mysecond();
  for (i = 0; i < TRIALS; i++) {
    memset(matrix_r, 0, sizeof(double)*MSIZE*MSIZE);
    if (use_optimized) {
      multiply_matrices_optimized();
    } else {
      multiply_matrices_naive();
    }
  }
  t2 = mysecond();
  // use the results for avoiding problem with smart compiler
  double sum = average_result();
  printf("3. Sum = %8.6f \n", sum); 
  printf("4. time = %f\n", (t2 - t1)/(double)TRIALS);

  return 0;
}
