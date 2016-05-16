#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#define MAX 512

int tGenerator, uClock, id=1;

struct carInfo {
    int idCar;
    char dest;
    int parkingTime;
};

int nDigits(int n)
{
    int nDig = 0;
    
    while(n > 0)
    {
        n /= 10;
        nDig++;
    }
    
    return nDig;
}

int printToLog (int startTime, int idCar, char dest, int parkingTime, int tVida, char* obs)
{
    FILE* log = fopen("gerador.log", "w");
    
    fprintf(log, "Hello\n");
    /*
    
    int nSpaces = 8 - nDigits(startTime);
    
    for(; nSpaces > 0; --nSpaces)
    {
        fprintf(log, " ");
    }
    
    fprintf(log, startTime);
    fprintf(log, " ; ");
    
    nSpaces = 8 - nDigits(idCar);
    
    for(; nSpaces > 0; --nSpaces)
    {
        fprintf(log, " ");
    }
    
    fprintf(log, idCar);
    fprintf(log, " ;        ");
    fprintf(log, dest);
    fprintf(log, " ; ");
    
    nSpaces = 8 - nDigits(parkingTime);
    
    for(; nSpaces > 0; --nSpaces)
    {
        fprintf(log, " ");
    }
    
    fprintf(log, " ; ");
    
    nSpaces = 8 - nDigits(tVida);
    
    for(; nSpaces > 0; --nSpaces)
    {
        fprintf(log, " ");
    }
    
    fprintf(log, tVida);
    
    fprintf(log, " ; ");
    fprintf(log, obs);
    fprintf(log, "\n");
    */
    return 0;
}

void sleepTicks(numberOfTicks){
    
    clock_t start, end;
    start = clock();
    do{
       
        end = clock();       
    }while((end-start) < numberOfTicks);
}

void *lifeCycle(void *car){
    clock_t start, end; 
    start = clock();
    
    struct carInfo *info = (struct carInfo *) car;
    
    
    int idCar = info->idCar;
    int parkingTime = info->parkingTime;
    char dest = info->dest;
    int tVida = -1;
    char obs[MAX];
    
    sprintf(obs, "TEMP");    
    printToLog(start, idCar, dest, parkingTime, tVida, obs);
    
    printf("thread finished\n");
       
}

//TODO change all exit's 'magic' numbers

int main(int argc, char *argv[]){
    
    
    //Checks for number of arguments
    if(argc != 3)
    {
        printf("Invalid number of arguments!\n Expected 2, was %d\n", argc - 1);
        perror("Argument Number");
        exit(1);
    }
    
    //Possivelmente testar se os argumentos indicados sao inteiros
    tGenerator = atoi(argv[1]);
    uClock = atoi(argv[2]);
   
   printf("tGenerator: %d\n uClock: %d\n", tGenerator, uClock);
   
   
    
    //Create new empty file to store info
	FILE* log = fopen("gerador.log", "w");
    fprintf(log, "t(ticks) ; id_viat ; destino ; t_estac ; t_vida  ; observs\n");
    
    fclose(log);
    
    //Sets up a random number generator seed
    srand(time(NULL));


    //Sets up needed variables
    int randEntry, randSleep, randParkingMultiple;
    int parkingTime, sleepTime;
    int idCar;
    
    int counter = 0;
    
   
    time_t start, current;
    time(&start);
   
    do{ 
 
        
        //sleeps for some time before creating new car
         
        printf("Starting rands\n"); 
         
        randSleep = rand() % 10;
        
        if(randSleep < 0 && randSleep > 9){
            printf("Invalid sleepTime generation. Check rand code\n");
            perror("Rand sleep generation\n");
            exit(2); 
        }
        
        else if(randSleep <= 4){
            sleepTime = 0;
        }
        else if(randSleep <= 7){
            sleepTime = uClock;
        }
        else{
            sleepTime = 2 * uClock;
        }
        
        printf("Sleeping for %d ticks\n", sleepTime);

        
        sleepTicks(sleepTime);
        
        //Create new car
        
        idCar = id++;
        
        char entryFifoName;
        randEntry = rand() % 4;
        
        switch(randEntry){
            case 0:
                entryFifoName = 'N';
                break;
            case 1:
                entryFifoName = 'S';
                break;
            case 2:
                entryFifoName = 'E';
                break;
            case 3:
                entryFifoName = 'W';
                break;
            default:
                printf("Invalid entry generation. Check rand code\n");
                perror("Rand entry generation\n");
                exit(2);  
        }
        
        randParkingMultiple = rand() % 10 + 1;
        parkingTime = randParkingMultiple * uClock;
        
        pthread_t t;
        
        struct carInfo car;
        car.idCar = idCar;
        car.dest = entryFifoName;
        car.parkingTime = parkingTime;
        printf("creating thread\n");
        
        if((pthread_create(&t, NULL, &lifeCycle, (void *) &car)) != 0){
            printf("Error creating new car thread\n");
            perror("Creating thread");
            exit(4);
        }
        
                
        //TODO
        //Store information in generator.log
        
        time(&current);
    } while(difftime(current,start) < tGenerator);
    
   
    printf("Generator finished its work!\n");
    
    
    
    return 0;
    //TODO
    //Show log
    
    
}