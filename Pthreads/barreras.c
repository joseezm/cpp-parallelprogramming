#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>

int thread_count;
int counter = 0;
pthread_mutex_t barrier_mutex;

sem_t count_sem;
sem_t barrier_sem;

pthread_mutex_t mutex;
pthread_cond_t cond_var;

void* B_busymutex(void* rank){
    pthread_mutex_lock(&barrier_mutex);
    counter++;
    pthread_mutex_unlock(&barrier_mutex);
    while(counter < thread_count);
    return NULL;
}

void* B_semaforo(void* rank){
    sem_wait(&count_sem);
    if(counter == thread_count-1){
        counter = 0;
        sem_post(&count_sem);
        int j;
        for(j=0;j<thread_count-1;j++){
            sem_post(&barrier_sem);
        }
    }
    else{
        counter++;
        sem_post(&count_sem);
        sem_wait(&barrier_sem);
    }

    return NULL;
}

void* B_var(void* rank){
    pthread_mutex_lock(&mutex);
    counter++;
    if (counter == thread_count){
        counter = 0;
        pthread_cond_broadcast(&cond_var);
    }
    else{
        while(pthread_cond_wait(&cond_var, &mutex) != 0);
    }
    pthread_mutex_unlock(&mutex);
    
    return NULL;
}


int main(int argc, char* argv[]){ 

    sem_init(&count_sem, 0, 1);
    sem_init(&barrier_sem, 0, 0);
    long thread;
    pthread_t * thread_handles;
    thread_count = strtol(argv[1], NULL, 10);
    thread_handles = malloc (thread_count * sizeof(pthread_t));
    
    long i;
    for(i =0;i<thread_count;i++)
        pthread_create(&thread_handles[i], NULL, B_semaforo, (void*)i); 

    for(i=0;i<thread_count;i++)
        pthread_join(thread_handles[i], NULL);

    free (thread_handles);    
    return 0;
    
}