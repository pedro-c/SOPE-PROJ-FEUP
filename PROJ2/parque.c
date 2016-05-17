#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#define MAX 512

int n_lugares, t_abertura;

void *controlador(char identificador){
        
        //TODO
        //Criar fifo proprio ex: 'fifoN'
        
    
    
   }

int main(int argc, char *argv[]){

    //arg1 -> n_lugares
    //arg2 -> t_abertura

    //Checks for number of arguments
    if(argc != 3)
    {
        printf("Invalid number of arguments!\n Expected 2, was %d\n", argc - 1);
        perror("Argument Number");
        exit(1);
    }
      
    //converts inputs from string to int and saves in global variable           
    n_lugares = atoi(argv[1]);
    t_abertura = atoi(argv[2]);
   
   //checks if inputs are valid (more than zero)
    if(n_lugares < 1){
        printf("error: n_lugares < 1");
        perror("n_lugares must be bigger than 0");
        exit(1);
    } 
    if(t_abertura < 1){
        printf("error: t_abertura < 1");
        perror("t_abertura must be bigger than 0");
        exit(1);
    } 
    
    time_t start, current;
    //saves the starting time
    time(&start);
    
    //creates thread 'controlador' N
        if((pthread_create(&t, &attr, &controlador, 'N')) != 0){
            printf("Error creating new 'controlador' thread\n");
            perror("Creating thread");
            exit(4);
        }
        
        //creates thread 'controlador' S
        if((pthread_create(&t, &attr, &controlador, 'N')) != 0){
            printf("Error creating new 'controlador' thread\n");
            perror("Creating thread");
            exit(4);
        }
        
        //creates thread 'controlador' E
        if((pthread_create(&t, &attr, &controlador, 'N')) != 0){
            printf("Error creating new 'controlador' thread\n");
            perror("Creating thread");
            exit(4);
        }
        
        //creates thread 'controlador' W
        if((pthread_create(&t, &attr, &controlador, 'N')) != 0){
            printf("Error creating new 'controlador' thread\n");
            perror("Creating thread");
            exit(4);
        }
    
    do{  
        
        //saves current time
        time(&current);  
    }while(difftime(current,start) < t_abertura) //compares current time with starting time and checks if it is time to close the park
    
    
    
      
    
    


}