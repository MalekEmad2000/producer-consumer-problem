#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>
 
 // Data structure to represent a circular queue to use as a buffer
typedef struct queue
{
    int *items;     // array to store queue elements
    int maxsize;    // maximum capacity of the queue
    int front;      // front points to the front element in the queue (if any)
    int rear;       // rear points to the last element in the queue
    int size;       // current capacity of the queue
}queue;
 
// function to initialize a queue
struct queue* newQueue(int size)
{
    struct queue *pt = NULL;
    pt = (struct queue*)malloc(sizeof(struct queue));
 
    pt->items = (int*)malloc(size * sizeof(int));
    pt->maxsize = size;
    pt->front = 0;
    pt->rear = -1;
    pt->size = 0;
 
    return pt;
}
 
//  function to return the size of the queue
int size(struct queue *pt) {
    return pt->size;
}
 
// function to check if the queue is empty 
int isEmpty(struct queue *pt) {
    return !size(pt);
}
 
// function to return the front element of the queue
int front(struct queue *pt)
{
    if (isEmpty(pt))
    {
        printf("Underflow\nProgram Terminated\n");
        exit(EXIT_FAILURE);
    }
 
    return pt->items[pt->front];
}
 
//  function to add an element `x` to the queue
void enqueue(struct queue *pt, int x)
{
    if (size(pt) == pt->maxsize)
    {
        printf("Overflow\nProgram Terminated\n");
        exit(EXIT_FAILURE);
    }
 
    
 
    pt->rear = (pt->rear + 1) % pt->maxsize;    // circular queue
    pt->items[pt->rear] = x;
    pt->size++;
 
     
}
 
// function to dequeue the front element
void dequeue(struct queue *pt)
{
    if (isEmpty(pt))    // front == rear
    {
        printf("Underflow\nProgram Terminated\n");
        exit(EXIT_FAILURE);
    }
 
    
 
    pt->front = (pt->front + 1) % pt->maxsize;  // circular queue
    pt->size--;
 
   
}
 // global buffer and semaphores (shared memory)
 
  
 const int buffer_size=3;
 const int N=5;    // number od counter threads
 int counter=0;
 queue *buffer; // buffer 
 
 
 
 sem_t counter_semaphore;   // semaphore for mutual exclusion between counter and monitor threads
 sem_t s;
 sem_t n;
 sem_t size_b;    
 
 
 // monitor (producer)
void *monitor_function(){
 
 while(1){

int value;   // save value 
int wait_counter; 
sem_getvalue(&counter_semaphore, &wait_counter); 

if(wait_counter<=0)
printf("monitor Thread %li waiting to read counter\n",pthread_self()); 

 
sem_wait(&counter_semaphore);
 value=counter; 
 printf("monitor Thread %li reading a count value of %d\n",pthread_self(),value); 
counter=0;                              // critical section
sem_post(&counter_semaphore);



int full; 
sem_getvalue(&size_b, &full); 
if(full==0)
printf("buffer is full \n");

sem_wait(&size_b);
sem_wait(&s);
enqueue(buffer,value);                //critical section
printf("monitor inserted value %d in buffer ,buffer size is %d\n",value,size(buffer));
sem_post(&s);
sem_post(&n);

int random_number=rand() %10;
sleep(random_number);



 }
 
 }
// collector (consumer)
void *collector_function(){
 
 while(1){
 
 
 // sleep 
int random_number=rand() %10;
sleep(random_number);

// do work

// check if buffer is empty
int empty; 
sem_getvalue(&size_b, &empty); 
if(empty==buffer_size){
printf("collector thread: buffer is empty  \n");
}

sem_wait(&n);
sem_wait(&s);
printf("collector thread reading from buffer value %d\n",front(buffer));
dequeue(buffer);    // critical section
sem_post(&s);
sem_post(&size_b);





 

 }
 
 }
 //counter
 void *counter_function(){
   
   
  
 while(1){
 
 printf("counter Thread %li recieved a message\n",pthread_self());
 
int wait_counter; 
sem_getvalue(&counter_semaphore, &wait_counter); 

if(wait_counter<=0)
printf("counter Thread %li is waiting to write value of semaphore is %d \n",pthread_self(),wait_counter); 
 
 sem_wait(&counter_semaphore);
 counter++;
 printf("counter Thread %li is now adding to counter value of counter is %d\n",pthread_self(),counter); 
 sem_post(&counter_semaphore);
 
 int random_number=rand() %5;
 sleep(random_number);



 }
 
 }
 
 
 void main (){
 
buffer= newQueue(buffer_size);
 
 // counter semaphore to allow only one access at a time
sem_init(&counter_semaphore,0,1);
sem_init(&s,0,1);                           //  for mutual exclusion
sem_init(&n,0,0);                           //  for empty buffer
sem_init(&size_b,0,buffer_size);            //  no of empty spaces in buffer
 
 // monitor thread 
 
 pthread_t monitor ;
 pthread_create(&monitor,NULL,&monitor_function,NULL);
  
 // collector thread 
 pthread_t collector;
 pthread_create(&monitor,NULL,&collector_function,NULL);
  
 // counter threads
 pthread_t counter[N];
 
 
 for(int i=0;i<N;i++){
 
 pthread_create(&counter[i],NULL,&counter_function,NULL);
 
 
 }
 
 
 
 pthread_join(monitor,NULL);
 pthread_join(collector,NULL);

for(int i=0;i<N;i++){
 
 pthread_join(counter[i],NULL);
 
 
 } 
 
 
 
}


 
