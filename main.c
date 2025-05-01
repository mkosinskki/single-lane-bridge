#include <stdio.h>
#include <stdlib.h> //lib with exit statuses

int main (int argc, char *argv[])
{
    if(argc != 2)
    {
        printf("Usage: %s N\n", argv[0]);
        exit(EXIT_FAILURE);
    }
}