#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define N 5 
#define LEFT (i-1+N)%N 
#define RIGHT (i+1)%N
#define MAX_MEALS 5

#define THINKING 0
#define HUNGRY 1
#define EATING 2

int status[N] = {0};

sem_t mutex;
sem_t s[N];

void Eat(int i) {
    printf("Философ %d ест\n", i);
    sleep(rand() % 2 + 1);
}

void Think(int i) {
    printf("Философ %d размышляет...\n", i);
    sleep(rand() % 3 + 1);
}

void Check_eat(int i) {
    if(status[i] == HUNGRY && status[LEFT] != EATING && status[RIGHT] != EATING) {
        status[i] = EATING;
        sem_post(&s[i]);
    }
}

void Take_Forks(int i) {
    sem_wait(&mutex);
    status[i] = HUNGRY;
    printf("Философ %d голоден\n", i);
    Check_eat(i);
    sem_post(&mutex);
    sem_wait(&s[i]);
}

void Put_Forks(int i) {
    sem_wait(&mutex);
    status[i] = THINKING;
    printf("Философ %d закончил есть и положил вилки\n", i);
    Check_eat(LEFT);
    Check_eat(RIGHT);
    sem_post(&mutex);
}

void* Philosopher(void* arg) {
    int i = (int)(long)arg;
    
    for(int meals = 0; meals < MAX_MEALS; meals++) {
        Think(i);
        Take_Forks(i);
        Eat(i);
        Put_Forks(i);
    }
    printf("Философ %d закончил есть \n", i);
    return NULL;
}

int main() {
    pthread_t philosophers[N];
    
    srand(time(NULL));
   
    sem_init(&mutex, 0, 1);
    for(int i = 0; i < N; i++) {
        sem_init(&s[i], 0, 0);
    }
    
    for(int i = 0; i < N; i++) {
        pthread_create(&philosophers[i], NULL, Philosopher, (void*)(long)i);
    }
    
    for(int i = 0; i < N; i++) {
        pthread_join(philosophers[i], NULL);
    }
    
    sem_destroy(&mutex);
    for(int i = 0; i < N; i++) {
        sem_destroy(&s[i]);
    }
    
    printf("Все философы наелись!\n");
    return 0;
}
