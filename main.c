#include <stdio.h>
#include <stdlib.h> //lib with exit statuses
#include <pthread.h>
#include <stdbool.h>

#define timeInCity = 5000
#define timeOnTheBridge = 1000

pthread_mutex_t bridgeOccupied = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t valuesEdit = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t carSetup = PTHREAD_MUTEX_INITIALIZER;

int count = 0;
int aQueue = 0;
int bQueue = 0;
int inACity = 0;
int inBCity = 0;

typedef struct Car {
    int carNumber;
    bool location;
} Car;

void *carThread(void *passedCar);
void logEmpty();
void log(int carNumber, int fromLocation);


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

    count = N;

    pthread_t *carTab;
    carTab = calloc(N, sizeof(pthread_t));

    srand(time(NULL));

    for(int a=0; a< N; a++)
    {
        Car *car = malloc(sizeof(Car));
        car->carNumber = a;
        car->location = rand()%2;
        pthread_create(&(carTab[a]), NULL, carThread, car);
    }

    for(int b=0; b<N; b++)
    {
        pthread_join(carTab[b], NULL);
    }

    free(carTab);
    return 0;
}

void *carThread(void *passedCar)
{
    Car *car = (Car *)passedCar;
    int carNumber = car->carNumber;
    int city = car->location;

    // pthread_mutex_lock
}

void logEmpty()
{

}

void log(int carNumber, int fromLocation)
{

}