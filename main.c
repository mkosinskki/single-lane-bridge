#include <stdio.h>
#include <stdlib.h> //lib with exit statuses
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

#define timeInCity 5000
#define timeOnTheBridge 1000
#define maxPassesBeforeSwitch 5

pthread_mutex_t bridgeOccupied = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t valuesEdit = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t carSetup = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t directionLock = PTHREAD_MUTEX_INITIALIZER;

int count = 0;
int aQueue = 0;
int bQueue = 0;
int inACity = 0;
int inBCity = 0;
int carsPassedInCurrentDirection = 0;

int currentDirection = -1;

typedef struct Car {
    int carNumber;
    bool location; // 0 = A, 1 = B
} Car;

void carInit(Car *car);
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
        printf("Cars count must be at least 1\n");
        exit(EXIT_FAILURE);
    }

    count = N;
    pthread_mutex_lock(&carSetup);

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

void carInit(Car *car)
{
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

    if(inACity + inBCity == count)
    {
        pthread_mutex_unlock(&carSetup);
    }
    pthread_mutex_lock(&carSetup);
    pthread_mutex_unlock(&carSetup);
}

void *carThread(void *passedCar) 
{
    Car *car = (Car *)passedCar;
    int carNumber = car->carNumber;
    int city = car->location;

    carInit(car);

    while(1) 
    {
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

        while (1) 
        {
            pthread_mutex_lock(&directionLock);
            bool canGo = (currentDirection == -1 || currentDirection == city);

            if (canGo && pthread_mutex_trylock(&bridgeOccupied) == 0) 
            {
                if (currentDirection == -1) 
                {
                    currentDirection = city;
                    carsPassedInCurrentDirection = 0;
                }
                pthread_mutex_unlock(&directionLock);
                break;
            }
            pthread_mutex_unlock(&directionLock);
            usleep(1000);
        }

        pthread_mutex_lock(&valuesEdit);
        if (city == 0) aQueue--;
        else bQueue--;
        logCar(carNumber, city);
        city = (city + 1) % 2;
        car->location = city;
        pthread_mutex_unlock(&valuesEdit);

        usleep(timeOnTheBridge);


        pthread_mutex_lock(&valuesEdit);
        if (city == 0) inACity++;
        else inBCity++;
        logEmpty();
        pthread_mutex_unlock(&valuesEdit);

        pthread_mutex_lock(&directionLock);
        carsPassedInCurrentDirection++;

        if (carsPassedInCurrentDirection >= maxPassesBeforeSwitch) 
        {
            if ((currentDirection == 0 && bQueue > 0) || (currentDirection == 1 && aQueue > 0)) 
            {
                currentDirection = 1 - currentDirection;
                carsPassedInCurrentDirection = 0;
            } 
            else 
            {
                carsPassedInCurrentDirection = 0;
            }
        }

        if ((currentDirection == 0 && bQueue > 0 && aQueue == 0) || (currentDirection == 1 && aQueue > 0 && bQueue == 0)) 
        {
            currentDirection = 1 - currentDirection;
            carsPassedInCurrentDirection = 0;
        }

        if (aQueue == 0 && bQueue == 0) 
        {
            currentDirection = -1;
            carsPassedInCurrentDirection = 0;
        }

        pthread_mutex_unlock(&directionLock);
        pthread_mutex_unlock(&bridgeOccupied);

        usleep(timeInCity);
    }

    free(car);
    return NULL;
}

void logEmpty()
{
    printf("A-%d %d>>> [ EMPTY ] <<<%d %d-B\n", inACity, aQueue, bQueue, inBCity);
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

    printf("A-%d %d>>> [ %s %d %s ] <<<%d %d-B\n", inACity, aQueue, sign, carNumber, sign, bQueue, inBCity);
}