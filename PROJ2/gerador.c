#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <semaphore.h>
#define SEM "/semaf"
#define MAX 512

//Exit code for invalid arguments
#define ARG_EXIT 1
//Exit code for error in Random Number Generation
#define RAND_EXIT 2
//Exit code for error in thread creation
#define THREAD_EXIT 3


int tGenerator, uClock, id=1;

//Pointer to generator.log file
FILE* gLog;


pthread_mutex_t logLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t idLock = PTHREAD_MUTEX_INITIALIZER;



/**
 * Struct with the relevant information to be passed to thread function
 */
struct carInfo {
	int idCar;
	char dest;
	int parkingTime;
};

/**
 * Function to read a line from the specified fifo
 * @param fd Fifo to be read
 * @param str pointer to the string which will store what is read from the fifo
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
	if(n == 0)
		return 1;
	int nDig = 0;

	while(n > 0)
	{
		n /= 10;
		nDig++;
	}

	return nDig;
}

/**
 * Prints the car information to gerador.log
 * @param startTime Current time
 * @param idCar Car ID
 * @param dest Destination of the car (park entrance)
 * @param parkingTime Time the car will/has be/been parked
 * @param obs Observation, i.e., if the car entered, exited, the park is full, etc. 
 */
int printToLog (int startTime, int idCar, char dest, int parkingTime, int tVida, char* obs)
{
	int nSpaces = 10 - nDigits(startTime);

	for(; nSpaces > 0; --nSpaces)
	{
		fprintf(gLog, " ");
	}
	fprintf(gLog, "%d ;", startTime);

	nSpaces = 10 - nDigits(idCar);
	for(; nSpaces > 0; --nSpaces)
	{
		fprintf(gLog, " ");
	}
	fprintf(gLog, "%d ;         %c ;", idCar, dest);

	nSpaces = 10 - nDigits(parkingTime);
	for(; nSpaces > 0; --nSpaces)
	{
		fprintf(gLog, " ");
	}
	fprintf(gLog, "%d ;", parkingTime);

	if(tVida == -1)
	{
		fprintf(gLog, "        ? ;   ");
	}
	else
	{
		nSpaces = 9 - nDigits(tVida);
		for(; nSpaces > 0; --nSpaces)
		{
			fprintf(gLog, " ");
		}
		fprintf(gLog, "%d ; ", tVida);
	}

	fprintf(gLog, "%s\n", obs);


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
 * Vehicle tracker thread function
 * Sends information of a car to the entrance controller and waits for a response
 * @param car Struct containg the car's information
 */
void *lifeCycle(void *car){
	clock_t start, end;
	//Register time where the car was created
	start = clock();

	//Creates intermediary struct to extract values from the "car" parameter
	struct carInfo *info = malloc (sizeof (struct carInfo));
	info = (struct carInfo *) car;
	
	//Passes parameter's values to new variables for easier handling
	int idCar = info->idCar;
	int parkingTime = info->parkingTime;
	char dest = info->dest;
	int tVida = -1;
	char obs[MAX] = "TMP";
	
	//Unlocks the writting of the car's values
	pthread_mutex_unlock(&idLock); 

	//Locks for writting to generator.log
	pthread_mutex_lock(&logLock); 

	int fd, messagelen;
	char message[100];
	
	//Name of the fifo used to communicate with the entrance's controller
	char * fifoName = malloc (sizeof (char));
	
	//Name of the "personal" fifo for each car
	char * fifoCar = malloc (sizeof (char));

	//Creates name for the car's fifo based on its unique ID
	sprintf(fifoCar, "car%d", idCar);

	//Creates fifo's name based on the destination
	switch(dest)
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
	
	//Semaphore to avoid writting to fifo at the same time as the main function in parque.c and other threads
	
	sem_t *semaphore = sem_open(SEM, O_CREAT ,0660,1);
	sem_wait(semaphore);
	
	//Opens fifo in write mode to write car's information
	fd=open(fifoName,O_WRONLY);
	if(fd!=-1){
		
		//Creates message to be writen in controller's fifo
		sprintf(message,"%d %d" , idCar, parkingTime);
		messagelen=strlen(message)+1;
		
		//Writes to fifo
		write(fd,message,messagelen);
		
		//Closes fifo
		close(fd);
		sem_post(semaphore);
		
		//Opens car's fifo for reading, will wait for an answer from park
		int fdA;
		char str[100];
		fdA = open(fifoCar, O_RDONLY);
		
		//Reads a line from the car's fifo and saves it in str
		readline(fdA,str);
		

		//Prints to log with the answer received
		printToLog(start, idCar, dest, parkingTime, tVida, str);
		
		//Unlocks writing in log
		pthread_mutex_unlock(&logLock);
		
		//If the answer received was that the car entered the park:
		if(strcmp(str,"Entrou!")==0){
			
				
			//Waits for another answer: the "exit" answer
			readline(fdA,str);
			
			//Locks writing in log 
			pthread_mutex_lock(&logLock);
			end = clock();
			
			//Calculates the time elapsed since the car was created
			tVida=end-start;

			printToLog(end, idCar, dest, parkingTime, tVida, str);
			
			//Unlocks writing in log
			pthread_mutex_unlock(&logLock);
			
			//Closes car's fifo
			close(fdA);
		}
	
		
	}
	//If the answer received meant that the park was full or closed
	else 
	{
			sem_post(semaphore);
	}

}

/** Main generator function
 * Generates cars that will attempt to enter the park
 * Receives 2 arguments: the time of operation for the park and the number of ticks the generator will work with
 */

int main(int argc, char *argv[]){

	//Checks for number of arguments
	if(argc != 3)
	{
		printf("Invalid arguments!\n Correct usage: gerador <int operation_time> <int generator clock ticks>\n");
		perror("Argument Number");
		exit(ARG_EXIT);
	}

	//Stores parameters in global variables
	tGenerator = atoi(argv[1]);
	uClock = atoi(argv[2]);

	//Checks if inputs are valid
	if(tGenerator < 1){
		printf("error: tGenerator < 1");
		perror("tGenerator must be an integer bigger than 0");
		exit(ARG_EXIT);
	}
	if(uClock < 1){
		printf("error: uClock < 1");
		perror("uClock must be an integer bigger than 0");
		exit(ARG_EXIT);
	}


	//Create new empty file for the log
	gLog = fopen("gerador.log", "w");
	
	//Prints the header
	fprintf(gLog, "  t(ticks) ;   id_viat ;   destino ;   t_estac ;   t_vida ;   observs\n");

	//Sets up a random number generator seed
	srand(time(NULL));

	//Sets up needed variables
	int randEntry, randSleep, randParkingMultiple;
	int parkingTime, sleepTime;
	int idCar;
	int counter = 0;
	time_t start, current;

	//Registers time because we will need it to stop the generator
	time(&start);

	do{
		//Creates a pseudo random number of ticks that the generator will sleep
		randSleep = rand() % 10;
		if(randSleep < 0 && randSleep > 9){
			printf("Invalid sleepTime generation. Check rand code\n");
			perror("Rand sleep generation");
			exit(RAND_EXIT);
		}
		
		//The generator will sleep for 0 (50% chance), 1 (30% chance) or 2 (20% chance) times the generator clock's ticks
		else if(randSleep <= 4){
			sleepTime = 0;
		}
		else if(randSleep <= 7){
			sleepTime = uClock;
		}
		else{
			sleepTime = 2 * uClock;
		}
		
		//Sleeps
		sleepTicks(sleepTime);

		//Create new car
		char entryFifoName;
		
		//Randomly picks an entrance for the car
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
			perror("Rand entry generation");
			exit(RAND_EXIT);
		}

		//Randomly generates the time the car will be parked
		randParkingMultiple = rand() % 10 + 1;
		
		//Parking time is equally likely to be any multiple 1-10 of the generator clock's ticks
		parkingTime = randParkingMultiple * uClock;


		//Sets up thread variables, including preparing the Detached Mode atrribute
		pthread_t t;
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

		pthread_mutex_lock(&idLock); //bloqueia a edição da struct car

		//Stores car's values
		idCar = id++;
		struct carInfo car;
		car.idCar = idCar;
		car.dest = entryFifoName;
		car.parkingTime = parkingTime;

		//Creates thread tracker for each car
		if((pthread_create(&t, &attr, &lifeCycle, (void *) &car)) != 0){
			printf("Error creating new car thread\n");
			perror("Creating thread");
			exit(THREAD_EXIT);
		}
		
		//Registers current time
		time(&current);
		
		//Checks if it's time to stop the generator
	} while(difftime(current,start) < tGenerator);


	printf("Generator finished successfully!\n");

	//Waits for all threads to be done
	pthread_exit(NULL);
	
	//Closes generator.log
	fclose(gLog);
	
	return 0;

}

