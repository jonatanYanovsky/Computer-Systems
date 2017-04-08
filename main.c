#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>


// built by Jonatan Yanovsky on April 8, 2017
// compiled with command: 	gcc -o p1 main.c -lm -lpthread


typedef enum { false, true } bool; 
// taken from http://stackoverflow.com/questions/1921539/using-boolean-values-in-c


struct threadData{
	int num;
	bool * myArray;
};


void* program1(void * data);
void* reversable(void * data);
int reverseDigits(int num);


int main(int argc, char *argv[]) {

	int n = atoi(argv[1]);

	if (n <= 1) {
		printf("Error, input must be greater than 1! \n"); // input from command line
		exit(1);
	}

	bool *A = malloc((n + 1) * sizeof(*A)); // array: 0 to n
	for (int i = 0; i < n+1; i++) 
		A[i] = true; 			// initialized to true

	struct threadData myData;
	myData.num = n;				
	myData.myArray = A; 	// create struct

	pthread_t tid; 	// declare thread

	// find primes
	int err = pthread_create(&tid, NULL, program1, (void *) &myData); // create thread, pass in struct

        if (err != 0) {
		printf("Thread error: %d \n", err); 	// check if error recieved
		exit(1);
	}

	pthread_join(tid, NULL); 			// wait for thread to finish

	printf("Primes less than sqrt(n): ");

	for (int i = 2; i <= sqrt(n); i++) {
		if (A[i] == true) 			// output modified array contents
			printf("%d ", i);
	}

	// check primeness if digits are in reverse 
	err = pthread_create(&tid, NULL, reversable, (void *) &myData); // create thread, pass in struct

        if (err != 0) {
		printf("Thread error: %d \n", err); 	// check if error recieved
		exit(1);
	}

	pthread_join(tid, NULL); 			// wait for thread to finish

	printf("\n");

	free(A);
	return 0;
}


void* program1(void * data) {	

	struct threadData *contents = (struct threadData *) data; // cast to the struct

	int num = contents->num; // get num

	for (int i = 2; i <= sqrt(num); i++) {
		if (contents->myArray[i] == true) {
			for (int j = i*i; j <= num; j += i)
				contents->myArray[j] = false; // do math
		}
	}

	pthread_exit(NULL); // exit with success code
}


void* reversable(void * data) {

	printf("\nPrime Reversable Numbers: ");

	struct threadData *contents = (struct threadData *) data; // cast to the struct

	int num = contents->num; // get num
	int reversedDigits = 0;
	
	for (int i = 2; i <= sqrt(num); i++) {	// for all numbers
		if (contents->myArray[i] == true) {	// if the number is prime
			reversedDigits = reverseDigits(i);	// reverse the digits		

			if (contents->myArray[reversedDigits] == true) // if the reverse number is prime
				printf("%d ", i);
		}
	}

	pthread_exit(NULL); // exit with success code
}


// taken from: http://www.geeksforgeeks.org/write-a-c-program-to-reverse-digits-of-a-number/
int reverseDigits(int num) {
    int rev_num = 0;
    while(num > 0)
    {
        rev_num = rev_num*10 + num%10;
        num = num/10;
    }
    return rev_num;
}


