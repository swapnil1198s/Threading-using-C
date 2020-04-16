#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<assert.h>
typedef struct __node_t {
    char *value;
    struct __node_t *next;
}node_t;

typedef struct __queue_t{
    node_t *head;
    node_t *tail;
    int size;
    pthread_mutex_t head_lock, tail_lock;
}queue_t;

void Queue_Init(queue_t *q){
    node_t *tmp = malloc(sizeof(node_t));
    q->size = 0;
    tmp->next = NULL;
    q->head = q->tail = tmp;
    pthread_mutex_init(&q->head_lock, NULL);
    pthread_mutex_init(&q->tail_lock, NULL);
}

void Queue_Enqueue(queue_t *q, char *value){
    node_t *tmp = malloc(sizeof(node_t));
    assert(tmp != NULL);
    tmp->value = value;
    tmp->next = NULL;
    pthread_mutex_lock(&q->tail_lock);
    q->tail->next = tmp;
    q->tail = tmp;
    
    pthread_mutex_unlock(&q->tail_lock);
}

int Queue_Dequeue(queue_t *q, char *value){
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
    q->size = q->size - 1;
    return 0;
}

int main(int argc, char *argv[])
{
    
    queue_t *lines;
    Queue_Init(lines);
    char sen[] = "";
    FILE *inFile;
    inFile = fopen("test.txt", "r");
    printf("adding sentences to queue: ");
    while(fgets(sen, 1000, inFile)!=NULL){
        Queue_Enqueue(lines, sen);
        lines->size = lines->size + 1;
        
    }

    for(int i=0; i<lines->size; i++){
        char *val = lines->head->value;
        printf("%s", val);
        Queue_Dequeue(lines, val);
    }
    fclose(inFile);
    return 0;
}