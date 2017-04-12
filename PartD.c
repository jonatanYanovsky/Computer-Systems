#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>


// Global shared memory variables
static int *glob_varMax;
static int *glob_varSum;
static int *glob_varMin;
static int *kill_myself; // signal to kill all processes

static int numberOfElements = 50;


void handle_signal(int signal) {

	const char *signal_name;

	if (signal == SIGINT) {
	    	printf("ProcessID %d Caught SIGINT, exiting now\n", getpid());
		*kill_myself = 1;
		exit(0);
	}
	else if (signal == SIGTSTP) {
	    	printf("ProcessID %d Caught SIGTSTP, exiting now\n", getpid());
		*kill_myself = 1;
		exit(0);
	}
	else if (signal == SIGTERM) {
	    	printf("ProcessID %d Caught SIGTERM, exiting now\n", getpid());
		*kill_myself = 1;
		exit(0);
	}
	else {
	    	printf("ProcessID %d Caught an unknown signal, exiting now\n", getpid());
		*kill_myself = 1;
		exit(0);
	}

}



void detectSignals() {
	signal(SIGINT, handle_signal);
	signal(SIGTSTP, handle_signal);
	signal(SIGTERM, handle_signal);
	
	if (*kill_myself == 1) {
		printf("One or more child processes have been terminated - ProcessID %d has permission to die, exiting now\n", getpid());
		exit(0);
	}

}


// Calculate the sum of the section
int sectionSum(int integers[numberOfElements], int counter) {
	int sum = 0, i;
	while (1) detectSignals(); // infinite while loop to simulate cpu-intensive operation
		// a) CTR+C
		// b) custom input signal, such as the letter 'q'
		// c) terminated by kill command 
	

	for (i = counter; i < counter+(numberOfElements/5); i++) {
		sum = sum + integers[i];
	}
	return sum;
}

// Calculate the min of the section
int sectionMin(int integers[numberOfElements], int counter) {
	int min = integers[counter], i;

	while (1) detectSignals();

	for (i = counter; i < counter+(numberOfElements/5); i++) {
		if (min > integers[i]) min = integers[i];
	}
	return min;
}

// Calculate the max of the section
int sectionMax(int integers[numberOfElements], int counter) {
	int max = integers[counter], i;

	while (1) detectSignals();

	for (i = counter; i < counter+(numberOfElements/5); i++) {
		if (max < integers[i]) max = integers[i];
	}
	return max;
}

int main(int argc, char* argv[]) {

	


	// Initialize shared memory
	glob_varMax = mmap(NULL, sizeof *glob_varMax, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	*glob_varMax = 0;

	glob_varSum = mmap(NULL, sizeof *glob_varSum, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	*glob_varSum = 0;

	glob_varMin = mmap(NULL, sizeof *glob_varMin, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	*glob_varMin = 1000;

	kill_myself = mmap(NULL, sizeof *kill_myself, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	*kill_myself = 0; 

	// Read in integers from 'input.txt'
	FILE *fileIn = fopen("input.txt", "r");
	int integers[numberOfElements];
	int i = 0;
	int num;
	while (fscanf(fileIn, "%d", &num) > 0) {
		integers[i] = num;
		i++;
	}
	fclose(fileIn);

	// Open file output 'outputD.txt'
	FILE *fileOut = fopen("outputD.txt", "w");

	// Multiprocessing
	int pid;
	pid = fork();

	if(pid == 0) {
		pid = fork();
		if(pid == 0) {
			pid = fork();
			if(pid == 0) {
					printf("Hi I'm process %d and my parent is %d.\n", getpid(), getppid());

					int max = sectionMax(integers,0);
					int sum = sectionSum(integers,0);
					int min = sectionMin(integers,0);

					if (max > *glob_varMax) *glob_varMax = max;
					if (min < *glob_varMin) *glob_varMin = min;
					*glob_varSum += sum;
					
					wait(NULL);
					printf("Max=%d\n", *glob_varMax);
					printf("Min=%d\n", *glob_varMin);
					printf("Sum=%d\n", *glob_varSum);
					
					exit(0);
			} else if(pid > 0) {
				printf("Hi I'm process %d and my parent is %d.\n", getpid(), getppid());

				int max = sectionMax(integers,(numberOfElements/5));
				int sum = sectionSum(integers,(numberOfElements/5));
				int min = sectionMin(integers,(numberOfElements/5));

				if (max > *glob_varMax) *glob_varMax = max;
				if (min < *glob_varMin) *glob_varMin = min;
				*glob_varSum += sum;

				exit(0);
			}
		} else if(pid > 0) {
			printf("Hi I'm process %d and my parent is %d.\n", getpid(), getppid());

			int max = sectionMax(integers,(2*numberOfElements/5));
			int sum = sectionSum(integers,(2*numberOfElements/5));
			int min = sectionMin(integers,(2*numberOfElements/5));

			if (max > *glob_varMax) *glob_varMax = max;
			if (min < *glob_varMin) *glob_varMin = min;
			*glob_varSum += sum;

			exit(0);
		}
	} else if(pid > 0) {
		pid = fork();
		if(pid == 0) {
				printf("Hi I'm process %d and my parent is %d.\n",getpid(), getppid());

				int max = sectionMax(integers,(3*numberOfElements/5));
				int sum = sectionSum(integers,(3*numberOfElements/5));
				int min = sectionMin(integers,(3*numberOfElements/5));

				if (max > *glob_varMax) *glob_varMax = max;
				if (min < *glob_varMin) *glob_varMin = min;
				*glob_varSum += sum;

				exit(0);
		} else if(pid > 0) {
			printf("Hi I'm process %d and my parent is %d.\n", getpid(), getppid());

			int max = sectionMax(integers,(4*numberOfElements/5));
			int sum = sectionSum(integers,(4*numberOfElements/5));
			int min = sectionMin(integers,(4*numberOfElements/5));

			if (max > *glob_varMax) *glob_varMax = max;
			if (min < *glob_varMin) *glob_varMin = min;
			*glob_varSum += sum;

			exit(0);
		
		}
	}

	// Close file output 'outputD.txt'
	fclose(fileOut);
	return 0;
}
