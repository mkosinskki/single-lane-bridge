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
    bool location; // 0 = A, 1 = B
} Car;

void *carThread(void *passedCar);
void logEmpty();
void logCar(int carNumber, int fromLocation);


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

    pthread_mutex_lock(&valuesEdit);
    if(city == 0)
    {
        inACity++;
    }
    else
    {
        inBCity++;
    }
    pthread_mutex_unlock(&valuesEdit);
}

void logEmpty()
{
    printf("A-%d %d>>> NIKT <<<%d %d-B\n", inACity, aQueue, bQueue, inBCity);
}

void logCar(int carNumber, int fromLocation)
{
    char *sign;
    if(fromLocation == 0)
    {
        sign = ">>>";
    }
    else
    {
        sign = "<<<";
    }

    printf("A-%d %d>>> %s %d %s <<<%d %d-B\n", inACity, aQueue, sign, carNumber, sign, bQueue, inBCity);
}