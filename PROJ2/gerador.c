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
	start = clock();

	//printf("Thread Starting\n");

	struct carInfo *info = malloc (sizeof (struct carInfo));
	info = (struct carInfo *) car;
	

	int idCar = info->idCar;
	int parkingTime = info->parkingTime;
	char dest = info->dest;
	int tVida = -1;
	char obs[MAX] = "TMP";
	pthread_mutex_unlock(&idLock); //liberta a edição da struct car no main

	pthread_mutex_lock(&logLock); //bloqueia a escrita no ficheiro para a thread atual

	printf("mutex logLock locked\n");
	int fd, messagelen;
	char message[100];
	char * fifoName = malloc (sizeof (char));
	char * fifoCar = malloc (sizeof (char));

	sprintf(fifoCar, "car%d", idCar);

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
	
	sem_t *semaphore = sem_open(SEM, O_CREAT ,0660,1);
	
	sem_wait(semaphore);
	
	printf("opening fifo\n");	
		
	fd=open(fifoName,O_WRONLY);
	if(fd!=-1){
		sprintf(message,"%d %d" , idCar, parkingTime);
		messagelen=strlen(message)+1;
		write(fd,message,messagelen);
		printf("message with id and parkingTime sent!\n");
		close(fd);
		sem_post(semaphore);
		
		int fdA;
		char str[100];
		fdA = open(fifoCar, O_RDONLY);
		printf("fdA opened\n");
		/*while(readline(fdA,str)){
			sscanf(str, str);	
		}*/
		readline(fdA,str);
		
		printf("printing to log!\n");
		printToLog(start, idCar, dest, parkingTime, tVida, str);	
		pthread_mutex_unlock(&logLock); //liberta a escrita no ficheiro
		if(strcmp(str,"Entrou!")==0){
			
				
			
			readline(fdA,str);
			pthread_mutex_lock(&logLock);
			end = clock();
			tVida=end-start;
			printf("printing to log2!\n");
			printToLog(end, idCar, dest, parkingTime, tVida, str);
			pthread_mutex_unlock(&logLock);
			close(fdA);
		}
		
	}else{
			sem_post(semaphore);
	}
	


	
	
}

//TODO change all exit's 'magic' numbers

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

	tGenerator = atoi(argv[1]);
	uClock = atoi(argv[2]);

	//checks if inputs are valid (more than zero)
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


	//Create new empty file to store info
	gLog = fopen("gerador.log", "w");
	fprintf(gLog, "  t(ticks) ;   id_viat ;   destino ;   t_estac ;   t_vida ;   observs\n");



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
			exit(RAND_EXIT);
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

		//printf("Sleeping for %d ticks\n", sleepTime);


		sleepTicks(sleepTime);

		//Create new car
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
			exit(RAND_EXIT);
		}

		randParkingMultiple = rand() % 10 + 1;
		parkingTime = randParkingMultiple * uClock;

		pthread_t t;
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

		pthread_mutex_lock(&idLock); //bloqueia a edição da struct car

		idCar = id++;
		struct carInfo car;
		car.idCar = idCar;
		car.dest = entryFifoName;
		car.parkingTime = parkingTime;

		if((pthread_create(&t, &attr, &lifeCycle, (void *) &car)) != 0){
			printf("Error creating new car thread\n");
			perror("Creating thread");
			exit(THREAD_EXIT);
		}
		time(&current);
		
	} while(difftime(current,start) < tGenerator);


	printf("Generator finished successfully!\n");

	pthread_exit(NULL);
	fclose(gLog);
	
	return 0;


}

