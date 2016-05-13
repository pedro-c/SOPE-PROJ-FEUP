#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#define MAX 512

int tGenerator, uClock, id=1;

int lifeCycle(int idCar, char dest, int parkingTime){
    
}


int main(argc, argv[]){
    
    
    //Checks for number of arguments
    if(argc != 3)
    {
        printf("Invalid number of arguments!\n Expected 2, was %d\n", argc - 1);
        perror("Argument Number");
        exit(1);
    }
    
    //Possivelmente testar se os argumentos indicados sao inteiros
    tGenerator = argv[1];
    uClock = argv[2];
    
    //TODO
    //Creates FIFO in write mode
    
    
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
         
        randSleep = rand() % 10;
        
        if(randSleep < 0 && randSleep > 9){
            printf("Invalid sleepTime generation. Check rand code\n");
            perror("Rand sleep generation\n");
            exit(2); 
        }
        
        else if(randSleep <= 4){
            sleepTime = 0; //Perguntar ao prof se 0 multiplos é 0 segundos
        }
        else if(randSleep <= 7){
            sleepTime = uClock;
        }
        else{
            sleepTime = 2 * uClock;
        }
        
        sleep(sleepTime);
        
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
        parkingTime = randParkingMultiple * uRelogio;
        
        //TODO
        //Launch thread for each new car
        //Store information in generator.log
        
        time(%current);
    } while(difftime(current, start) < tGenerator);
    
   
    printf("Generator finished its work!\n");
    
    //TODO
    //Close FIFO
    //Show log
    
    
}