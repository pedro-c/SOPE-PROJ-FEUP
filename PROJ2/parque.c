#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#define MAX 512

int n_lugares, t_abertura;


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
      
    //thread principal
          
    n_lugares = atoi(argv[1]);
    t_abertura = atoi(argv[2]);
   
    if(n_lugares < 1){
                   printf("n_lugares < 1");
        perror("n_lugares must be bigger than 0");
        exit(1);
    } 
    if(t_abertura < 1){
        printf("t_abertura < 1");
        perror("t_abertura must be bigger than 0");
        exit(1);
    } 
    
    time_t start, current;
    time(&start);
    
    do{
        
        
        
        
     time(&current);   
    }while(difftime(current,start) < t_abertura)
    
    
    
      
    
    


}