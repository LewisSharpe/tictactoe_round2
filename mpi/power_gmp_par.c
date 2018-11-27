/* 
   See: http://en.wikipedia.org/wiki/Power_series
   The Taylor series for the exponential function e^x:
    e^z = lim n|->inf \Sum_1^inf z^n / n!

   Basic GMP API usage is taken 
   from: http://www.cs.colorado.edu/~srirams/classes/doku.php/gmp_usage_tutorial
   Retrieved: 25/1/2013 
   GMP API: http://gmplib.org/manual/

   This is a general parallel solution to the above problem.
   It generates p-1 worker processes, computing the Taylor series with a
   stride of p-1, up to the point where the contribution drops below epsilon.
   Compared to the seq version, p-2 additional elements are computed.
   The master is not used during the main part of the computation, this could be improved.

   Compile: mpicc -UCHECK -UPRINT -lm -lgmp -O2 -o power_gmp_par power_gmp_par.c
   Run:     mpirun -n 7 power_gmp_par 4000 20

   Batch job of measurements + visualisation (cluster version up to 4x8 processors):
    > for ((i=2;i<32;i++)) ; do j=$[ $i - 1 ] ; echo "Workers: $j">>L0; mpirun -n ${i} power_gmp_par 4000 26 1>>L0 2>&1 ; done
    > cat L0 | sed -ne '/Workers/H;/Elapsed time:/H;${x;p}' | sed -e 's/^Workers: \([0-9]*\).*$/\1/' | sed -e 's/Elapsed time: \([.0-9]*\) secs /\1/' |  sed -e '/[.]/a\X' | sed ':a;N;$!ba;s/\n/ /g' | sed -e 's/X/\n/g'  > rt0.dat
    > echo "set term x11; plot 'rt0.dat' with lines ; pause 10" | gnuplot
*/

#include "gmp.h"
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>

// size of a string, containing the result of type mpq_t produced by one processor
// Q: Is this in general large enough? If not, how can you fix it?
#define GMP_STR_SIZE 640000

/* types */
typedef unsigned long int  ui;

/* prototypes */
static inline void fact(mpq_t res, ui n);
static inline void pow_int(mpq_t res, ui z, ui n) ;
static inline void power_e(mpq_t res, ui z, ui d) ;
static inline void power_e_step(mpq_t res, mpq_t eps, int *max_n, ui z, ui d, int from, int step, int id);

/* variables */
double startTime, stopTime;

/* aux functions */
// res = n!
static inline
void fact(mpq_t res, ui n){
  ui i;
  mpz_t p;
 
  mpz_init_set_ui(p,1); /* p = 1 */
  for (i=1l; i <= n ; ++i){
    mpz_mul_ui(p,p,i); /* p = p * i */
  }
  mpq_set_z(res, p);
  mpz_clear(p);
  // return res;
}

// res = z^n
static inline
void pow_int(mpq_t res, ui z, ui n) {
  mpz_t tmp, zq;
  mpz_init(tmp);
  mpz_init(zq);
  mpz_set_ui(zq,z);
  mpz_pow_ui(tmp, zq, n); /* tmp = z^n */
  mpq_set_z(res, tmp);    /* res = tmp */
  mpz_clear(tmp);
  mpz_clear(zq);
  // return res;
}

/* exponential function, result will be stored in res */
static inline 
void power_e(mpq_t res, ui z, ui d) {
  mpq_t sum, old_sum, eps;
  mpq_t tmp_num, tmp_den, tmp, tmpe, epsilon;
  ui n; 
  mpq_init(epsilon);
  mpq_init(eps);
  mpq_init(old_sum);
  mpq_init(sum);
  mpq_init(tmp_num);
  mpq_init(tmp_den);
  mpq_init(tmp);
  mpq_init(tmpe);
  // mpq_set_ui(epsilon,1l,1000000l); /* 1e-6 */
  pow_int(tmpe, 10l, d);              /* tmpe = 10^d */
  // mpq_set_z(epsilon, tmpe);        
  mpq_inv(epsilon, tmpe);             /* epsilon = 10^-d */
#if defined(DEBUG)
  fprintf(stderr, "\nPRECISION %d ie EPSILON: ",d);
  mpq_out_str(stderr, 10, epsilon);   
#endif
  mpq_set_ui(sum, 0l, 1l);
  mpq_set_ui(eps, 1l, 1l);
  for (n=0; mpq_cmp(eps, epsilon)>0; n++) {
#if defined(DEBUG)
    fprintf(stderr, "\nIT %d\t", n);
#endif
    mpq_set(old_sum, sum);
    pow_int(tmp_num, z, n);
    fact(tmp_den, n);
    mpq_div(tmp, tmp_num, tmp_den);  /* tmp = z^n / n! */
#if defined(DEBUG)
    fprintf(stderr, "\tBONZO \t");
    mpq_out_str(stderr, 10, tmp);   // mpq_out_str(stdout, 10, old_sum);   mpq_out_str(stdout, 10, eps);
#endif
    mpq_add(sum, sum, tmp);
    mpq_sub(eps, sum, old_sum);
#if defined(DEBUG)
    fprintf(stderr, "\tSUM \t");
    mpq_out_str(stderr, 10, sum);   // mpq_out_str(stdout, 10, old_sum);   mpq_out_str(stdout, 10, eps);
#endif
  }
  mpq_clear(tmp_num);
  mpq_clear(tmp_den);
  mpq_clear(tmp);
  mpq_clear(tmpe);
  mpq_clear(old_sum);
  mpq_set(res,sum);
  // return sum;
}

/* exponential function, result will be stored in res */
static inline 
void power_e_step(mpq_t res, mpq_t eps, int *max_n, ui z, ui d, int from, int step, int id) {
  mpq_t sum, old_sum;
  mpq_t tmp_num, tmp_den, tmp, tmpe, epsilon;
  ui n; 
  mpq_init(epsilon);
  mpq_init(old_sum);
  mpq_init(sum);
  mpq_init(tmp_num);
  mpq_init(tmp_den);
  mpq_init(tmp);
  mpq_init(tmpe);
  // mpq_set_ui(epsilon,1l,1000000l); // 1e-6
  pow_int(tmpe, 10l, d);              // 10^d
  // mpq_set_z(epsilon, tmpe);        
  mpq_inv(epsilon, tmpe);             // 10^-d
#if defined(DEBUG)
  fprintf(stderr, "\nPRECISION %d ie EPSILON: ",d);
  mpq_out_str(stderr, 10, epsilon);   
#endif
  mpq_set_ui(sum, 0l, 1l);
  mpq_set_ui(eps, 1l, 1l);
  for (n=from; mpq_cmp(eps, epsilon)>0; n+=step) {
#if defined(DEBUG)
    fprintf(stderr, "\n[%d] IT %d\t", id, n);
#endif
    mpq_set(old_sum, sum);
    pow_int(tmp_num, z, n);
    fact(tmp_den, n);
    mpq_div(tmp, tmp_num, tmp_den);
#if defined(DEBUG)
    fprintf(stderr, "\tBONZO \t");
    mpq_out_str(stderr, 10, tmp);   // mpq_out_str(stdout, 10, old_sum);   mpq_out_str(stdout, 10, eps);
#endif
    mpq_add(sum, sum, tmp);
    mpq_sub(eps, sum, old_sum);
#if defined(DEBUG)
    fprintf(stderr, "\tSUM \t");
    mpq_out_str(stderr, 10, sum);   // mpq_out_str(stdout, 10, old_sum);   mpq_out_str(stdout, 10, eps);
#endif
  }
  *max_n = n-step;
  mpq_clear(tmp_num);
  mpq_clear(tmp_den);
  mpq_clear(tmp);
  mpq_clear(tmpe);
  mpq_clear(old_sum);
  mpq_set(res,sum);
  // return sum;
}


int main (int argc, char **argv) {
  int p, id;
  long m, n, d, z;
  double elapsed_time;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &p);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);

  if (id == 0) { /*  master   --------------------------------- */
    if (argc<2) {
      fprintf(stderr, "Usage: power <z> <d> ... computes e^z with precision 10^-d, using a Taylor series");
      exit(1);
    }
    z = atol(argv[1]);
    d = atol(argv[2]);
    fprintf(stderr, "Computing e^%lu with %lu digit precision ...\n", z, d);
    fprintf(stderr, "Using 1 master and %d workers ...\n", p-1);
    fprintf(stderr, "Workers: %d\n", p-1);

    /* MAIN ------------------- */
    int i;
    char res_str[GMP_STR_SIZE]; 
    // double res, res1, res2, res_check;
    mpq_t result, res, epsilon, tmpe, resq_check;
    mpf_t resf;
    double res_check, time;

    // fprintf(stderr, "Computing e^%ld ...\n", z);
    mpq_init(res);
    mpq_init(result);
    mpq_init(epsilon);
    mpq_init(tmpe);
    mpf_init(resf);

    // mpq_set_ui(epsilon,1l,1000000l); // 1e-6
    pow_int(tmpe, 10l, d);              // 10^d
    // mpq_set_z(epsilon, tmpe);        
    mpq_inv(epsilon, tmpe);             // 10^-d

    // start the timer
    MPI_Barrier(MPI_COMM_WORLD);
    elapsed_time = - MPI_Wtime();

    /* General case: use p-1 workers, each computing every (p-1)-th element of the series */

    long from, step, last_n, max_n; 
    int len, l;
    double *times;
    MPI_Status status;

    max_n = (long)p-1;
    step=(long)(p-1);
    for (i=1; i<p; i++) { 
      from=(long)(i-1);
      MPI_Send(&z, 1, MPI_LONG, i, 0, MPI_COMM_WORLD);
      MPI_Send(&d, 1, MPI_LONG, i, 0, MPI_COMM_WORLD);
      MPI_Send(&from, 1, MPI_LONG, i, 0, MPI_COMM_WORLD);
      MPI_Send(&step, 1, MPI_LONG, i, 0, MPI_COMM_WORLD);
    }

    times = (double *)malloc(p*sizeof(double));
    for (i=1; i<p; i++) { 
      MPI_Recv(&len, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
      // fprintf(stderr, "[%d] len=%d\n", id, len);
      MPI_Recv(&res_str, len, MPI_CHAR, i, 0, MPI_COMM_WORLD, &status);
      // fprintf(stderr, "[%d] res_str=%s\n", id, res_str);
      MPI_Recv(&last_n, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
      // fprintf(stderr, "[%d] last_n=%d\n", id, last_n);
      MPI_Recv(&time, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &status);
      // fprintf(stderr, "[%d] time=%f\n", id, time);
      /* MPI_Recv(&l1, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status); */
      /* MPI_Recv(&eps1_str, l1, MPI_CHAR, 1, 0, MPI_COMM_WORLD, &status); */
      res_str[len] = '\0';
#if defined(DEBUG)
      fprintf(stderr, "[%d] FROM PE %d (as string) res=%s\n", id, i, res_str);
#endif
#if defined(CHECK)
      fprintf(stderr, "[%d] received result from PE %d, with last_n=%d\n", id, i, last_n);
#endif
      /* unmarshall the GMP data */
      if (gmp_sscanf(res_str,"%Qd",&res)==0) {
	fprintf(stderr,"[%d] Error in gmp_sscanf", id); 
	MPI_Abort(MPI_COMM_WORLD, 2);
      }
#if defined(DEBUG)
      gmp_fprintf(stderr, "[%d] FROM PE %d res=%Qd\n", id, i, res);
#endif
      // ress[i] = res;
      times[i] = time;
      max_n = (last_n>max_n) ? last_n : max_n;
      mpq_add(result, result, res);
    }

    /* Q: how can we determine in general that this is close enough to the solution? */

    // Maybe use an epsilon returned by the last PE to decide whether to compute more
    // mpq_set_ui(eps2, 1l, 1l); // eps2 = 1/1  

    m=0;
    /* Q: what if this is not close enough? */
    /* A: iterate over the rest, element by element, until we drop below epsilon */
    /* shouldn't be necessary in this version!
    while (mpq_cmp(eps2,epsilon)>0) {
      power_e(eps2, eps0, z, d, to2+1, to2+1, id);
      to2++;
      m++;
      mpq_set(res1,res);
      mpq_add(res, res1, eps2);
    }
    */
    // stop the timer
    elapsed_time += MPI_Wtime();

    // mpf_set_q(resf,eps2);
#if defined(CHECK)
    gmp_printf("\n[%d] Finished computation ----------------", id);
    // gmp_printf("\n[%d] Last n was %d (last %d done on master), last epsilon was %.18Ff", id, to2, m, resf);
#endif

    mpf_set_q(resf,result);
#if defined(PRINT)
    gmp_printf("\ne^%d = %Ff", (int)z, resf);
    // gmp_printf("\ne^%d = %Ff\n  or as rational %Qd", (int)z, resf, res);
    // mpq_out_str(stdout, 10, res);
#else
    printf("\ndone");
#endif
#if defined(CHECK)
    printf("\nPrecision: %d digits", d);
    if (z<=100) {
      mpf_t resf_check, eps_check, epsilon_f;
      mpf_init(resf_check);
      mpf_init(eps_check);
      mpf_init(epsilon_f);
      res_check = exp((double)z);
      printf("\nExpected result: %.9f", res_check);
      mpf_set_d(resf_check,res_check);
      mpf_sub(eps_check,resf_check,resf);
      mpf_abs(eps_check,eps_check);
      mpf_set_q(epsilon_f, epsilon);
      if (mpf_cmp(eps_check,epsilon_f)>0) 
	gmp_printf("\nWRONG result: difference from expected result is %.18Ff, larger than required epsilon %.18Ff", eps_check, epsilon_f);
      else	  
	gmp_printf("\nOK result: difference from expected result is %.18Ff, smaller than required epsilon %.18Ff", eps_check, epsilon_f);
      mpf_clear(resf_check);
      mpf_clear(eps_check);
      mpf_clear(epsilon_f);
    } else {
      // if the master computed the last iterations, print the epsilon
      mpq_t res_check;
      mpf_t resf_check, eps_check, epsilon_f;
      mpq_init(res_check);
      mpf_init(resf_check);
      mpf_init(eps_check);
      mpf_init(epsilon_f);
      startTime = clock();
      power_e(res_check, z, d);
      stopTime = clock();
      mpf_set_q(resf_check, res_check);
      gmp_printf("\nComparing result with result of seq computation (both using GMP)");
      if (mpf_cmp(resf_check,resf)!=0) 
	gmp_printf("\nWRONG result: %.22Ff\nSeq result: %.22Ff", resf, resf_check);
      else	  
	gmp_printf("\nOK result"); // : %.22Ff", resf);
      printf("\nSequential elapsed time: %f secs\n", 
	     (stopTime-startTime)/CLOCKS_PER_SEC);
      mpq_clear(res_check);
      mpf_clear(resf_check);
      mpf_clear(eps_check);
      mpf_clear(epsilon_f);
    } 
#endif
    printf("\nmax_n: %d ", max_n); 
    printf("\nElapsed time: %f secs \n by PEs: ", 
	   /*   n, res, res_check,*/ elapsed_time);
    for (i=1; i<p; i++) { 
      printf(", PE %d: %f secs", i, times[i]);
    }
  } else { /*  worker   --------------------------------- */
    MPI_Status status;
    int len, len0, max_n;
    long from, step;
    mpq_t res, eps;
    char buf[GMP_STR_SIZE], buf0[GMP_STR_SIZE]; // FIXME: EVIL CONSTANT

    mpq_init(res);
    mpq_init(eps);

    // start the timer
    MPI_Barrier(MPI_COMM_WORLD);
    // elapsed_time = - MPI_Wtime();

    MPI_Recv(&z, 1, MPI_LONG, 0, 0, MPI_COMM_WORLD, &status);
    MPI_Recv(&d, 1, MPI_LONG, 0, 0, MPI_COMM_WORLD, &status);
    MPI_Recv(&from, 1, MPI_LONG, 0, 0, MPI_COMM_WORLD, &status);
    MPI_Recv(&step, 1, MPI_LONG, 0, 0, MPI_COMM_WORLD, &status);

    // start the timer
    elapsed_time = - MPI_Wtime();

    fprintf(stderr, "[%d] from=%d, step=%d\t", id, from, step);
    power_e_step(res, eps, &max_n, z, d, from, step, id);            // result of our interval
    //gmp_fprintf(stderr, "[%d] res=%Qd", id, res);
    if ((len = gmp_sprintf(buf,"%Qd", res))==0 || len>=GMP_STR_SIZE) {      // marshall into a string
      fprintf(stderr,"[%d] Error in gmp_sprintf", id); 
      MPI_Abort(MPI_COMM_WORLD, 2);
    }
    /*
    if ((len0 = gmp_sprintf(buf0,"%Qd", eps))==0) {      // marshall into a string
      fprintf(stderr,"[%d] Error in gmp_sprintf", id); 
      MPI_Abort(MPI_COMM_WORLD, 2);
    }
    */
#if defined(CHECK)
    // fprintf(stderr, "[%d] last_n=%d, res=%s\n", id, max_n, buf);
    fprintf(stderr, "[%d] last_n=%d\n", id, max_n);
#endif

    // stop the timer
    elapsed_time += MPI_Wtime();

    MPI_Send(&len, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    MPI_Send(buf, len, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    MPI_Send(&max_n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    MPI_Send(&elapsed_time, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    /* MPI_Send(&len0, 1, MPI_INT, 0, 0, MPI_COMM_WORLD); */
    /* MPI_Send(buf0, len0, MPI_CHAR, 0, 0, MPI_COMM_WORLD); */
  }

  MPI_Finalize();
  return 0;
}


