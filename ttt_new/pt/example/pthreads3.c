// From: https://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html#BASICS
/* Orig: pthreads1.c from above web page; changed for arrays as input

Compute the sum over NUM_THREAD arrays (with 5 int values each) in NUM_THREAD separate threads, 
and compare the result with a sequential sum over both arrays. An example of passing arguments to
a tunable number of pthreads, capturing the threads in arrays and returning the results.

C compiler: gcc -g -DDEBUG -lpthread pthreads3.c
Run:        ./a.out
Debug:      gdb ./a.out

Results:

For NUM_THREADS 5:
Thread 0 return code 0 and result (in *(bufs[0]->res)) 10
[0] Local sum: 10	Accum so far: 10
Thread 1 return code 0 and result (in *(bufs[1]->res)) 35
[1] Local sum: 35	Accum so far: 45
Thread 2 return code 0 and result (in *(bufs[2]->res)) 60
[2] Local sum: 60	Accum so far: 105
Thread 3 return code 0 and result (in *(bufs[3]->res)) 85
[3] Local sum: 85	Accum so far: 190
Thread 4 return code 0 and result (in *(bufs[4]->res)) 110
[4] Local sum: 110	Accum so far: 300
Total sum: 300
Seqential sum: 300

*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 5

// a data-structure that is used as sole input argument to a function that is called via pthread_create
typedef struct {
  int *arr;
  int len;
  int *res;
} my_thread_args;

// unused (from orig example)
void *print_message_function( void *ptr );

// prototype for the function that will be called via pthread_create
void *do_sum(my_thread_args *buf);
// this would be the sequential fct interface
// void *do_sum( int *arr, int len, int *res );

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

main()
{
     // pthread_t thread1, thread2;
     // int arr1[] = { 1, 2, 3, 4, 5};
     //int arr2[] = { 6, 7, 8, 9, 10};
     const int len = 5;
     int  iret1, iret2;
     int i, sum;

     /* Create independent threads each of which will execute function */
     /* unused; from orig example
     iret1 = pthread_create( &thread1, NULL, print_message_function, (void*) arr1);
     iret2 = pthread_create( &thread2, NULL, print_message_function, (void*) arr2);
     */

     { // set-up arguments for 2 pthread_create calls; also reserve space for the return value
       int t;
       int rc[NUM_THREADS];
       my_thread_args *bufs[NUM_THREADS];
       pthread_t thr[NUM_THREADS];
       
       for (t=0; t<NUM_THREADS; t++) {
	 my_thread_args *buf;
	 const int asz = len*sizeof(int);          // size of array in bytes
	 const int bsz = sizeof(my_thread_args);   // size of buffer in bytes

	 int arr[] = { 1, 2, 3, 4, 5 };
	 for (i=0; i<len; i++) {
	   arr[i] = t*len+i;
	 }
				 
	 buf = (my_thread_args *)malloc(bsz);         // allocate buffer for argument to 1st thread
	 buf->arr = (int *)malloc(asz);               // need to malloc array as well!
	 memcpy(buf->arr, arr, asz);   		     // fill in buffer: array
	 buf->len = len;           		     // fill in len
	 buf->res = (int *)malloc(sizeof(int));       // last value is used to hold a pointer to the result!
	 bufs[t] = buf;
	 /*
	 in this example, buf1->res could be of type int, and in this case the last 2 mallocs wouldn't be needed;
	 in general, the result will be a structure, though, so this is a more general template
       */
       
#ifdef DEBUG
	 printf("[%d] buf @ %p; bufs[%d] @ %p; buf->arr @ %p; buf->res @ %p\n", t, buf, t, bufs[t], buf->arr, buf->res);
#endif
       
	 rc[t] = pthread_create( &thr[t], NULL, do_sum, (void*) buf);
       }

       /* Wait till threads are complete before main continues. Unless we  */
       /* wait we run the risk of executing an exit which will terminate   */
       /* the process and all threads before the threads have completed.   */
       
       for (t=0; t<NUM_THREADS; t++) {
	 pthread_join( thr[t], NULL);
	 printf("Thread %d return code %d and result (in *(bufs[%d]->res)) %d\n", t, rc[t], t, *(bufs[t]->res));
	 sum += *(bufs[t]->res);
	 printf("[%d] Local sum: %d\tAccum so far: %d\n", t, *(bufs[t]->res), sum);
       }
       printf("Total sum: %d\n", sum);

       // Test the result by doing a sequential sum over both arrays
       sum=0;
       for (t=0; t<NUM_THREADS; t++) {
	 for (i=0; i<len; i++) {
	   sum += bufs[t]->arr[i];
	 }
       }
       printf("Seqential sum: %d\n", sum);
     }

     exit(0);
}

// aux fcts, just for debugging
char *show_arr(int *arr, int len) {
  int i;
  char str[80];
  char *sstr = (char *)malloc(80*sizeof(char));

  strcpy(sstr, "");
  for (i=0; i<len; i++) {
    sprintf(str, "%s %d, ", sstr, arr[i]);
    strncpy(sstr, str, 80);
  }
  return sstr;
}

// printing like this in a thread is not a good choice: output will be interleaved with other thread's output
void print_arr(int *arr, int len) {
  int i;
  
  for (i=0; i<len; i++) {
    fprintf(stderr, "%d, ", arr[i]);
  }
  return;
}
				
// void *do_sum( int *arr, int len, int *res )
void *do_sum(my_thread_args *buf)
{
  int sum, i;
  int *arr = buf->arr;
  int len = buf->len;
  int *res = buf->res;
  sum = 0;
#ifdef DEBUG
  printf("do_sum: array @ %p\n", arr);
  printf("do_sum: len = %d\n", len);
  printf("ptr for result: %p\n", res);
  // printf("do_sum: array: %s\n", ""); print_arr(arr, len);
  printf("do_sum: array: %s\n", show_arr(arr, len));
#endif

  for (i=0; i<len; i++) {
    sum += arr[i];
  }
#ifdef DEBUG
  printf("do_sum: res = %d\n", sum);
#endif
  *(buf->res) = sum;
  // return sum;
}

void *print_message_function( void *ptr )
{
     char *message;
     message = (char *) ptr;
     printf("%s \n", message);
}
