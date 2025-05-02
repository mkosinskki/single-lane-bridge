#include <stdio.h>
#include <stdlib.h> //lib with exit statuses
#include <pthread.h>
#include <stdbool.h>

#define timeInCity = 5000
#define timeOnTheBridge = 1000

int aQueue = 0;
int bQueue = 0;
int carOnTheBridge = 0;
int inACity = 0;
int inBCity = 0;

typedef struct Car {
    int carNumber;
    int location;
    bool readyToCross;
} Car;

typedef struct CarQueue {
    Car *cars[100];
    int carFront;
    int carRear;
    int count;
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