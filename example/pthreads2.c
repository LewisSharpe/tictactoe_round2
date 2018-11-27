// From: https://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html#BASICS
/* Orig: pthreads1.c from above web page; changed for arrays as input

Compute the sum over two arrays (arr1 and arr2) in 2 separate threads, and compare the
result with a sequential sum over both arrays. An example of passing arguments to
pthreads and returning the results.

C compiler: gcc -g -DDEBUG -lpthread pthreads2.c
Run:        ./a.out
Debug:      gdb ./a.out
Results:

Thread 1 return code 0 and result (in *(buf1->res)) 15
Thread 2 return code 0 and result (in *(buf2->res)) 40
Total sum: 55
Seqential sum: 55

*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// a data-structure that is used as sole input argument to a function that is called via pthread_create
typedef struct {
  int *arr;
  int len;
  int *res;
} my_thread_args;
 
/*                              larr
                                |
                 +--------+     v 
  int *arr;      |    --------> [1,2,3,4,5]
  int len;       |    5   |
  int *res;      |   ---------> ____
                 +--------+
*/
// unused (from orig example)
void *print_message_function( void *ptr );

// prototype for the function that will be called via pthread_create
void *do_sum(my_thread_args *buf);
// this would be the sequential fct interface
// int do_sum( int *arr, int len);
// void *do_sum( int *arr, int len, int *res );
// [ 1, 2, 3] (len 3) -> 6
// ^ arr       ^ len    
// call: { int len, res; int arr[5];    do_sum(arr, len, &res);    }
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

main()
{
     pthread_t thread1, thread2;
     int arr1[] = { 1, 2, 3, 4, 5};
     int arr2[] = { 6, 7, 8, 9, 10};
     const int len = 5;
     int  iret1, iret2;
     int i, sum;

     /* Create independent threads each of which will execute function */
     /* unused; from orig example
     iret1 = pthread_create( &thread1, NULL, print_message_function, (void*) arr1);
     iret2 = pthread_create( &thread2, NULL, print_message_function, (void*) arr2);
     */

     { // set-up arguments for 2 pthread_create calls; also reserve space for the return value
       
       my_thread_args *buf1, *buf2;
       const int asz = len*sizeof(int);          // size of array in bytes
       const int bsz = sizeof(my_thread_args);   // size of buffer in bytes

       buf1 = (my_thread_args *)malloc(bsz);         // allocate buffer for argument to 1st thread
       buf2 = (my_thread_args *)malloc(bsz);         // allocate buffer for argument to 2nd thread
       buf1->arr = (int *)malloc(asz);               // need to malloc array as well!
       buf2->arr = (int *)malloc(asz);               // need to malloc array as well!
       memcpy(buf1->arr, arr1, asz);   		     // fill in buffer: array
       memcpy(buf2->arr, arr2, asz);   		     // fill in buffer: array
       buf1->len = len;           		     // fill in len
       buf2->len = len;                              // fill in len
       buf1->res = (int *)malloc(sizeof(int));       // last value is used to hold a pointer to the result!
       buf2->res = (int *)malloc(sizeof(int));       // last value is used to hold a pointer to the result!
       /*
	 in this example, buf1->res could be of type int, and in this case the last 2 mallocs wouldn't be needed;
	 in general, the result will be a structure, though, so this is a more general template
       */
       
#ifdef DEBUG
       printf("buf1 @ %p; buf1->arr @ %p; buf1->res @ %p\n", buf1, buf1->arr, buf1->res);
       printf("buf2 @ %p; buf2->arr @ %p; buf2->res @ %p\n", buf2, buf2->arr, buf2->res);
#endif
       
       iret1 = pthread_create( &thread1, NULL, do_sum, (void*) buf1);
       iret2 = pthread_create( &thread2, NULL, do_sum, (void*) buf2);
     
       /* Wait till threads are complete before main continues. Unless we  */
       /* wait we run the risk of executing an exit which will terminate   */
       /* the process and all threads before the threads have completed.   */
       
       pthread_join( thread1, NULL);
       pthread_join( thread2, NULL); 

       printf("Thread 1 return code %d and result (in *(buf1->res)) %d\n", iret1, *(buf1->res));
       printf("Thread 2 return code %d and result (in *(buf2->res)) %d\n", iret2, *(buf2->res));

       // printf("Total sum: %d\n", iret1+iret2);
       printf("Total sum: %d\n", *(buf1->res)+*(buf2->res));
     }

     // Test the result by doing a sequential sum over both arrays
     sum=0;
     for (i=0; i<len; i++) {
       sum += arr1[i]+arr2[i];
     }
     printf("Seqential sum: %d\n", sum);
     
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
  int *larr = buf->arr;
  int len = buf->len;
  int *res = buf->res;
  sum = 0;
#ifdef DEBUG
  printf("do_sum: array @ %p\n", larr);
  printf("do_sum: len = %d\n", len);
  printf("ptr for result: %p\n", res);
  // printf("do_sum: array: %s\n", ""); print_arr(larr, len);
  printf("do_sum: array: %s\n", show_arr(larr, len));
#endif

  for (i=0; i<len; i++) {
    sum += larr[i];
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

