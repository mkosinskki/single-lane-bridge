#include <stdio.h>
#include <stdlib.h> //lib with exit statuses
#include <semaphore.h>
#include <pthread.h>

typedef struct Car {
    int carNumber;
    int location;
    sem_t readyToCross;
} Car;

typedef struct CarQueue {
    Car *cars[100];
    int carFront;
    int carRear;
    int count;
    pthread_mutex_t mutex;
} CarQueue;

int main (int argc, char *argv[])
{
    if(argc != 2)
    {
        printf("Usage: %s N\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int N = atoi(argv[1]);
    if(N <= 0)
    {
        printf("Cars count must be at least 1");
        exit(EXIT_FAILURE);
    }
}