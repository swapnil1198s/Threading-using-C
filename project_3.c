#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<assert.h>
/*For some unknown reason I keep getting a segmentation fault in this program. 
I have tested the different methods and they work fine, but for some reason the queue initialization 
causes a fault*/

int totalWords = 0; //running total

typedef struct __node_t {
    struct __node_t *next;
    char value[1000];
}node_t;

typedef struct __queue_t{
    node_t *head;
    node_t *tail;
    int size;
    pthread_mutex_t head_lock, tail_lock;
}queue_t;

typedef struct __task_struct_t{
    int t_no;
    queue_t *sentences;
}task_struct_t;

//initialize the queue
//something in this function is causing the core dump
void Queue_Init(queue_t *q){
    node_t *tmp = malloc(sizeof(node_t));

    tmp->next = NULL;
    q->head = q->tail = tmp; 
    pthread_mutex_init(&q->head_lock, NULL);
    pthread_mutex_init(&q->tail_lock, NULL);
}

//add to queue
void Queue_Enqueue(queue_t *q, char value[]){
    node_t *tmp = malloc(sizeof(node_t));
    assert(tmp != NULL);
    strcpy(tmp->value, value);
    tmp->next = NULL;
    pthread_mutex_lock(&q->tail_lock);
    
    if(q->size == 0){
        q->head = q->tail = tmp;
    }
    else{
        q->tail->next = tmp;
        q->tail = tmp;
    }
    pthread_mutex_unlock(&q->tail_lock);
}

//remove from queue
int Queue_Dequeue(queue_t *q, char value[]){
    pthread_mutex_lock(&q->head_lock);
    node_t *tmp = q->head;
    node_t *new_head = tmp->next;
    if(new_head == NULL){
        pthread_mutex_unlock(&q->head_lock);
        return -1;
    }
    
    q->head = new_head;
    pthread_mutex_unlock(&q->head_lock);
    free(tmp);
    return 0;
}

//count words in a sentence
int countWords(char sen[]){
    int word = 0; //state determining if we are inside the word or outside
    int count = 0;
    int sum = 0;
    while(sen[count]!='\n'){
        if(sen[count] == ' ' || sen[count] == '\n' || sen[count] == '\t'){
            word = 1; //done with a word
        }
        else if(word == 1){
            sum ++;
            word = 0;
        }
    }
    totalWords = sum + totalWords;
    return sum;
}

//Consumer thread function
void *consumer(void *arg){
    task_struct_t *args = (task_struct_t *) arg;
    char tname[10];
    int num = args->t_no;
    queue_t *q = args->sentences;
    sprintf(tname, "%d", num);
    while(q->head->value != NULL){
        printf("Task %s", tname);
        pthread_mutex_lock(&q->head_lock);
        printf("Sentence: %s", q->head->value);
        int count = 0;
        count =  countWords(q->head->value);
        Queue_Dequeue(q,NULL);
        pthread_mutex_unlock(&q->head_lock);
    }
}

//start the consumer threads
void start_tasks(int num_of_tasks, queue_t *l){
    int num = 0;
    pthread_t consumers[num_of_tasks];
    queue_t *tasks;
    Queue_Init(tasks);
    while(num<(num_of_tasks)){
        char tname[10];
        sprintf(tname, "%d", num);
        task_struct_t *param = malloc(sizeof *param);
        param->t_no = num;
        param->sentences = l;
        assert(pthread_create(&consumers[num], NULL, consumer, &param)==0);
        num++;
    }
    for(int i =0; i<num_of_tasks;i++){
        assert(pthread_kill(consumers[i]));  
    }
}


int main(int argc, char *argv[])
{
    printf("Running program: %s", argv[0]);
    printf("number of consumer tasks: %s", argv[1]);

    queue_t *lines; //contains sentences 
    Queue_Init(lines);

    char sen[1000]; //sentence variable

//load in the data from file
    FILE *inFile;
    inFile = fopen("test.txt", "r"); 

//check if file was loaded correctly
     if(inFile==NULL){
        printf("File did not load");
        return 1;
    } 

//add the sentences from input to a queue
     printf("adding sentences to queue: ");
    while(fgets(sen, 1000, inFile)!=NULL){
        
        Queue_Enqueue(lines, sen);

        lines->size = lines->size + 1;
        printf("size: %d", lines->size);
    } 

//create the number of tasks specified
    printf("creating the tasks");
    int n = atoi(argv[1]);
    start_tasks(n, lines);
    printf("Total number of words: %d", totalWords); 
    fclose(inFile);
    return 0;
}