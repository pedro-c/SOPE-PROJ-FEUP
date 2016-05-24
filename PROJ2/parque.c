#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#define SEM "/semaf"
#define MAX 512

//Code to close the controllers
#define SHUTDOWN -1

//Exit code for invalid arguments
#define ARG_EXIT 1
//Exit code for error in Random Number Generation
#define RAND_EXIT 2
//Exit code for error in thread creation
#define THREAD_EXIT 3

#define FIFO_PERMISSIONS 0660


//Global variables to be used
//Opened = 1 means the park is opened, otherwise means it's closed
//nLug is the number of filled spots in the park
int n_lugares, t_abertura, opened=1, nLug=0;

//Pointer to parque.log file
FILE* pLog;

pthread_mutex_t nLugaresLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t nLugLock = PTHREAD_MUTEX_INITIALIZER;

//Struct with all the relevant info of each car

struct carAssistInfo {
	int idCar;
	int parkingTime;
};

/** Function to read a line from the specified fifo
 * @param fd Fifo to be read
 * @param str String to store the information read
*/

int readline(int fd, char *str)
{
	int n;
	do
	{
		n = read(fd,str,1);
	}
	while (n>0 && *str++ != '\0');


	return (n>0);
}

/**
 * Counts the number of digits in a number. Used in formatting the logs
 * @param Number to have its digits counted
 */
 
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


/**
 * Prints the car information to parque.log
 * @param t Current time
 * @param nlug Number of the spots taken in the park
 * @param id Car ID
 * @param obs Observation, i.e., if the car entered, exited, the park is full, etc. 
 */
 
int printToLog (int t, int nlug, int id, char* obs)
{
	int nSpaces = 10 - nDigits(t);

	for(; nSpaces > 0; --nSpaces)
	{
		fprintf(pLog, " ");
	}
	fprintf(pLog, "%d ;", t);

	nSpaces = 10 - nDigits(nlug);
	for(; nSpaces > 0; --nSpaces)
	{
		fprintf(pLog, " ");
	}
	fprintf(pLog, "%d ;         %d ;", nlug, id);


	fprintf(pLog, "%s\n", obs);


	return 0;
}

/**
 * Sleeps for an amount of ticks
 * @param numberOfTicks Number of ticks to sleep
 */

void sleepTicks(numberOfTicks){

	clock_t start, end;
	start = clock();
	do{

		end = clock();
	}while((end-start) < numberOfTicks);
}

/**
 * Car Assistant thread function
 * Accompanies the car and checks if he can enter the park
 * Also transmits information when he leaves
 * @param car Struct contain the car's information
 */

void *carAssistant(void *car){
	
	//Variable which will be used to store the time
	clock_t t;
	
	//Stores the car's information in new variables for easier handling
	struct carAssistInfo *info = (struct carInfo *) car;
	int idCar = info->idCar;
	int parkingTime = info->parkingTime;
	
	int messagelen;
	char message[100];
	
	//Creates a new fifo for the car using the car's unique ID
	char * fifoCar = malloc (sizeof (char));
	sprintf(fifoCar, "car%d", idCar);
	mkfifo(fifoCar,FIFO_PERMISSIONS);	

	//Opens the car's fifo for writing
	int fdA = open(fifoCar, O_WRONLY);
	
	//Locks the access to a critical section
	pthread_mutex_lock(&nLugaresLock); 
	
	//If the park is not full
	if(n_lugares> 0)
	{
		//If there is only one spot left the car enters and the park is now full
		if(n_lugares == 1){
			
			//Registers current time to print in log
			t = clock();
			pthread_mutex_lock(&nLugLock); 
			
			//Registers that there is one more car in the park
			nLug++;
			
			//Prints to log
			printToLog (t, nLug, idCar, "cheio");
			pthread_mutex_unlock(&nLugLock); 
		}else{
			
			//Registers current time to print in log
			t = clock();
			pthread_mutex_lock(&nLugLock); 
			
			//Registers that there is one more car in the park
			nLug++;
			
			//Prints to log
			printToLog (t, nLug, idCar, "estacionamento");
			pthread_mutex_unlock(&nLugLock); 
		}
		
		//Registers that there is one less available spot in the park
		n_lugares--;
		
		sprintf(message,"Entrou!");
		messagelen=strlen(message)+1;
		
		//Transmits to the vehicle tracker thread that the car has entered
		write(fdA,message,messagelen);
		pthread_mutex_unlock(&nLugaresLock);
		
		//The car is parked of the amount that it wants to be
		sleepTicks(parkingTime);

		
		
		pthread_mutex_lock(&nLugaresLock); 
		
		//The car leaves and there is another spot in the park
		n_lugares++; 
		pthread_mutex_unlock(&nLugaresLock);
		pthread_mutex_lock(&nLugLock);
		
		//Registers current time
		t = clock();
		nLug--;
		
		//If the park is opened, the car leaves, else prints that it is closed and won't receive any more cars
		if(opened==0){
			printToLog (t, nLug, idCar, "encerrado");
		}else{
			printToLog (t, nLug, idCar, "saida");
		}
		pthread_mutex_unlock(&nLugLock); 
		
		//Writes to the car's fifo that it left
		sprintf(message,"Saiu!");
		write(fdA,message,messagelen);
		close(fdA);
	}
	//If the park is full:
	else
	{
		
		sprintf(message,"Cheio!");
		messagelen=strlen(message)+1;
		write(fdA,message,messagelen);
		pthread_mutex_unlock(&nLugaresLock);
		close(fdA);
	}
}

/**
 * Entrance controller thread function
 * @param identificador Char indicating the entrance
 */
 
void *controlador(void* identificador){

	char * fifoName;

	switch( *(char *) identificador)
	{
	case 'N':
		fifoName = "fifoN";
		break;
	case 'S':
		fifoName = "fifoS";
		break;
	case 'E':
		fifoName = "fifoE";
		break;
	case 'W':
		fifoName = "fifoW";
		break;
	}

	int idCar, parkingTime;

	int fd;
	char str[100];

	//While the park is opened
	while(opened){
		
		
		//Opens the entrance's fifo and reads the next car's id and parking time
		fd = open(fifoName,O_RDONLY);
		while(readline(fd,str)){
			sscanf(str, "%d %d", &idCar, &parkingTime);
		}
		
		close(fd);

		//Sets up thread variables and attributes
		pthread_t t;
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

		//If the controller receives idCar == -1, then it closes the park
		if(idCar == SHUTDOWN){
			opened = 0;
			break;
		}
		//Elses processes the car and creates a new carAssistant thread
		else
		{
			struct carAssistInfo carAssist;
			carAssist.idCar = idCar;
			carAssist.parkingTime = parkingTime;
			pthread_create(&t, &attr, &carAssistant, (void *) &carAssist );
		}
	 }
}

int main(int argc, char *argv[]){
	
	//Creates semaphore that will be used to avoid writting at the same time as other threads
	sem_t *semaphore = sem_open(SEM, O_CREAT ,0660,1);
	
	//Checks for number of arguments
	if(argc != 3)
	{
		printf("Invalid number of arguments!\n Correct usage: parque <int num_Lugares> <int open_time>\nn");
		perror("Argument Number");
		exit(1);
	}

	//converts inputs from string to int and saves in global variable
	n_lugares = atoi(argv[1]);
	t_abertura = atoi(argv[2]);

	//checks if inputs are valid
	if(n_lugares < 1){
		printf("error: n_lugares < 1");
		perror("n_lugares must be an integer bigger than 0");
		exit(1);
	}
	if(t_abertura < 1){
		printf("error: t_abertura < 1");
		perror("t_abertura must be an integer bigger than 0");
		exit(1);
	}

	time_t start, current;
	//Saves the starting time
	time(&start);

	//Creates fifos for each entrance
	mkfifo("fifoN",FIFO_PERMISSIONS);
	mkfifo("fifoS",FIFO_PERMISSIONS);
	mkfifo("fifoE",FIFO_PERMISSIONS);
	mkfifo("fifoW",FIFO_PERMISSIONS);

	
	//A char for each of the entrances, used in creating controller threads
	char NN = 'N';
	char SS = 'S';
	char EE = 'E';
	char WW = 'W';
	
	//Creates the parque.log file
	pLog = fopen("parque.log", "w");
	
	//Prints the log header
	fprintf(pLog, "  t(ticks) ;   nLug    ;   id_viat ;   observs\n");

	
	//Prepares thread variable and attributes
	pthread_t t;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);


	//Creates thread 'controlador' North
	if((pthread_create(&t, &attr, &controlador, &NN)) != 0){
		printf("Error creating new 'controlador' thread\n");
		perror("Creating thread");
		exit(THREAD_EXIT);
	}

	//Creates thread 'controlador' South
	if((pthread_create(&t, &attr, &controlador, &SS)) != 0){
		printf("Error creating new 'controlador' thread\n");
		perror("Creating thread");
		exit(THREAD_EXIT);
	}

	//Creates thread 'controlador' East
	if((pthread_create(&t, &attr, &controlador, &EE)) != 0){
		printf("Error creating new 'controlador' thread\n");
		perror("Creating thread");
		exit(THREAD_EXIT);
	}

	//Creates thread 'controlador' West
	if((pthread_create(&t, &attr, &controlador, &WW)) != 0){
		printf("Error creating new 'controlador' thread\n");
		perror("Creating thread");
		exit(THREAD_EXIT);
	}
	
	//Sleeps for the duration that it is supposed to be opened
	sleep(t_abertura);
	
	//Only this or a thread can write to the controller's fifo at any given time
	sem_wait(semaphore);
	
	//Opens controllers' fifos for writing
	int fdN = open("fifoN", O_WRONLY);
	int fdS = open("fifoS", O_WRONLY);
	int fdE = open("fifoE", O_WRONLY);
	int fdW = open("fifoW", O_WRONLY);
	
	int messagelen;
	char message[100];
	
	//Prepares message to be delivered to controllers
	sprintf(message,"%d %d" , SHUTDOWN, SHUTDOWN);
	messagelen=strlen(message)+1;
	
	//Writes SHUTDOWN command to the controllers' fifos
	write(fdN,message,messagelen);
	write(fdS,message,messagelen);
	write(fdE,message,messagelen);
	write(fdW,message,messagelen);
	
	//Unlocks
	sem_post(semaphore);

	printf("Park Closed\n");
	pthread_exit(NULL);

	return 0;
}
