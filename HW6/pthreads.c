#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>

// Define gloabal varibles that can be accessed by threads
pthread_mutex_t lock;
int string_length;
volatile int counter = 0;

// Define struct to keep track of string and thread id
typedef struct {

    int thread_id;
    char *str;

} my_struct_t;

// Define thread function
void *mythread(void *arg) {

    // Initilize structure and choose char value
    my_struct_t *mys = arg;
    int asciiValue = 36;

    // Loop the number of times spesified and add character to string 
    for(counter = counter; counter < string_length; counter++) {  
        assert(pthread_mutex_lock(&lock) == 0);  
        *(mys->str + counter) = asciiValue; 

        assert(pthread_mutex_unlock(&lock) == 0);  
    
    }  
    return NULL;
}


int main(int argc, char *argv[])
{
    // Make sure  the appropriate arguments were given by the user
    if (argc != 3) {
	fprintf(stderr, "ERROR: Please enter appropriate parameters <num_threads> <length> \n");
	exit(1);
    }
	
    int num_threads = atoi(argv[1]);
    int length = atoi(argv[2]);

    printf("Number of threads: %d\n", num_threads);
    printf("Length of threads: %d\n\n", length);

    // Define struct and the length of the strings and associated pointer
    my_struct_t my_string[num_threads];
    string_length = length;
    char string[string_length];
    char *pointer = string;

    // Allocate memory for string 
    memset(string, '\0', sizeof(char) * string_length);
 
    // Identify the thread
    pthread_t threads[num_threads];

    // Loop n number of times and create n number of threads
    for(int i = 0; i < num_threads; i++){

        my_string[i].thread_id = i;
        my_string[i].str = pointer;
    
        if (pthread_create(&threads[i], NULL, mythread, &my_string[i]) != 0) {
            printf("Unable to create thread\n");
            exit(1);
        } 
    }

    // Join threads
    for(int i = 0; i < num_threads; i++){
        assert(pthread_join(threads[i], NULL ) == 0);
    }
	
    printf("Output: <%s>\n", string);

    return -1;
}
