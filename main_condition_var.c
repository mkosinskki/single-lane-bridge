#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

#define timeInCity 5000
#define timeOnTheBridge 1000
#define maxPassesBeforeSwitch 2

pthread_mutex_t valuesEdit = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t carSetup = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t bridge_cond = PTHREAD_COND_INITIALIZER;

int count = 0; //all cars count
int aQueue = 0; //cars in queue from A to B
int bQueue = 0; //cars in queue from B to A
int inACity = 0; //cars waiting in city A
int inBCity = 0; //cars waiting in city B
int bridge_busy = 0;
int carsPassedInCurrentDirection = 0;
int currentDirection = -1;

//car struct - single car stores own number and location
typedef struct Car {
    int carNumber;
    bool location; // 0 = A, 1 = B
} Car;

void carInit(Car *car);
void *carThread(void *passedCar);
void logEmpty();
void logCar(int carNumber, int fromLocation);

//function that checks args and starts threads
int main (int argc, char *argv[])
{
    //only 1 arg is accepted (without including the name)
    if(argc != 2)
    {
        printf("Usage: %s N\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    //checking quantity 
    int N = atoi(argv[1]);
    if(N <= 0)
    {
        printf("Cars count must be at least 1");
        exit(EXIT_FAILURE);
    }

    count = N;
    pthread_mutex_lock(&carSetup);

    pthread_t *carTab;
    carTab = calloc(N, sizeof(pthread_t));

    srand(time(NULL));

    //threads creating with carThread fun as starting fun
    for(int a=0; a< N; a++)
    {
        Car *car = malloc(sizeof(Car));
        car->carNumber = a;
        car->location = rand()%2;
        pthread_create(&(carTab[a]), NULL, carThread, car);
    }

    //waiting till the end of threads
    for(int b=0; b<N; b++)
    {
        pthread_join(carTab[b], NULL);
    }

    free(carTab);
    return 0;
}

//function that sets values of cars in cities, and syncs cars start
void carInit(Car *car)
{
    int carNumber = car->carNumber;
    int city = car->location;

    //editing car values in citis based on drawn location
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

    //synchronization of the start of all cars
    if(inACity + inBCity == count)
    {
        pthread_mutex_unlock(&carSetup);
    }
    pthread_mutex_lock(&carSetup);
    pthread_mutex_unlock(&carSetup);
}

//main thread function with all car logic. The function supports mechanisms to prevent starvation by changing bridge direction
void *carThread(void *passedCar)
{
    Car *car = (Car *)passedCar;
    int carNumber = car->carNumber;
    int city = car->location;

    //start synchronization 
    carInit(car);

    //main car logic, inifinite loop
    while (1)
    {
        //locking mutex before editing data like cars in city or in queue
        pthread_mutex_lock(&valuesEdit);
        if (city == 0) 
        {
            aQueue++;
            inACity--;
        } 
        else 
        {
            bQueue++;
            inBCity--;
        }
        logEmpty();
        pthread_mutex_unlock(&valuesEdit);
        pthread_mutex_lock(&valuesEdit);

        //car waits until the bridge is free, the direction matches, and passing limits allow it.
        while (bridge_busy || (currentDirection != -1 && currentDirection != city) || 
            (carsPassedInCurrentDirection >= maxPassesBeforeSwitch &&
            ((city == 0 && bQueue > 0) || (city == 1 && aQueue > 0)))) 
        {
            pthread_cond_wait(&bridge_cond, &valuesEdit);
        }

        bridge_busy = 1;

        if (currentDirection == -1) 
        {
            currentDirection = city;
            carsPassedInCurrentDirection = 0;
        }

        carsPassedInCurrentDirection++;

        if (city == 0)
        {
            aQueue--;
        }
        else
        {
            bQueue--;
        }
        logCar(carNumber, city);

        //updating location
        city = (city + 1) % 2;
        car->location = city;
        pthread_mutex_unlock(&valuesEdit);

        //simulation of bridge crossing
        usleep(timeOnTheBridge);

        pthread_mutex_lock(&valuesEdit);

        bridge_busy = 0;

        if (city == 0)
        {
            inACity++;
        }
        else
        {
            inBCity++;
        }
        logEmpty();

        //logic of changing bridge direction
        //changes direction based on maxPassesBeforeSwitch (means that if x cars passed direction is changed)
        if (carsPassedInCurrentDirection >= maxPassesBeforeSwitch && ((currentDirection == 0 && bQueue > 0) || (currentDirection == 1 && aQueue > 0))) 
        {
            currentDirection = 1 - currentDirection;
            carsPassedInCurrentDirection = 0;
        }

        //if current queue is empty, bridge direction is changed
        if ((currentDirection == 0 && aQueue == 0 && bQueue > 0) || (currentDirection == 1 && bQueue == 0 && aQueue > 0)) 
        {
            currentDirection = 1 - currentDirection;
            carsPassedInCurrentDirection = 0;
        }

        //if both queues are empty, currentDirection is being reset
        if (aQueue == 0 && bQueue == 0) 
        {
            currentDirection = -1;
            carsPassedInCurrentDirection = 0;
        }

        pthread_cond_broadcast(&bridge_cond);
        pthread_mutex_unlock(&valuesEdit);

        //simulation of waiting in city
        usleep(timeInCity);
    }
    free(car);
    return NULL;
}

//function that logs changes like new car in queue or in city
void logEmpty()
{
    printf("A-%d %d>>> [ EMPTY ] <<<%d %d-B\n", inACity, aQueue, bQueue, inBCity);
}

//function that logs car crossing the bridge
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

    printf("A-%d %d>>> [ %s %d %s ] <<<%d %d-B\n", inACity, aQueue, sign, carNumber, sign, bQueue, inBCity);
}