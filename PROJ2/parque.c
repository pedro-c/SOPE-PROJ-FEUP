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

int n_lugares, t_abertura, opened, nLug=0;

FILE* pLog;

pthread_mutex_t nLugaresLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t nLugLock = PTHREAD_MUTEX_INITIALIZER;

struct carAssistInfo {
	int idCar;
	int parkingTime;
};

int readline(int fd, char *str);

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

void sleepTicks(numberOfTicks){

	clock_t start, end;
	start = clock();
	do{

		end = clock();
	}while((end-start) < numberOfTicks);
}

void *carAssistant(void *car){
	clock_t t;
	printf("Car assistant created\n");
	struct carAssistInfo *info = (struct carInfo *) car;
	int idCar = info->idCar;
	int parkingTime = info->parkingTime;
	int messagelen;
	char message[100];
	char * fifoCar = malloc (sizeof (char));

	sprintf(fifoCar, "car%d", idCar);
	mkfifo(fifoCar,0660);	

	int fdA = open(fifoCar, O_WRONLY);
	pthread_mutex_lock(&nLugaresLock); 
	if(n_lugares> 0){
		if(n_lugares == 1){
			t = clock();
			pthread_mutex_lock(&nLugLock); 
			nLug++;
			printToLog (t, nLug, idCar, "cheio");
			pthread_mutex_unlock(&nLugLock); 
		}else{
			t = clock();
			pthread_mutex_lock(&nLugLock); 
			nLug++;
			printToLog (t, nLug, idCar, "estacionamento");
			pthread_mutex_unlock(&nLugLock); 
		}
		
		n_lugares--;
		sprintf(message,"Entrou!");
		messagelen=strlen(message)+1;
		
		write(fdA,message,messagelen);
		pthread_mutex_unlock(&nLugaresLock);
		
		sem_t *semaphore = sem_open(SEM,O_CREAT, 0660,1);
		
		sleepTicks(parkingTime);
		sem_post(semaphore);
		sem_close(semaphore);
		
		pthread_mutex_lock(&nLugaresLock); 
		n_lugares++;
		pthread_mutex_unlock(&nLugaresLock);
		pthread_mutex_lock(&nLugLock); 
		t = clock();
		nLug--;
		if(opened==0){
			printToLog (t, nLug, idCar, "encerrado");
		}else{
			printToLog (t, nLug, idCar, "saida");
		}
		pthread_mutex_unlock(&nLugLock); 
		
		sprintf(message,"Saiu!");
		write(fdA,message,messagelen);
		close(fdA);
	}else{
		
		sprintf(message,"Cheio!");
		messagelen=strlen(message)+1;
		write(fdA,message,messagelen);
		pthread_mutex_unlock(&nLugaresLock);
		close(fdA);

	}
	 
}

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


	while(1){
		fd = open(fifoName,O_RDONLY);
		while(readline(fd,str)){
			sscanf(str, "%d %d", &idCar, &parkingTime);
			printf("ID Carro: %d\n", idCar);
			printf("Parking Time: %d\n", parkingTime);
			printf("Entrada: %c\n", *(char *) identificador);

		}
		close(fd);
	

	
		pthread_t t;
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);


		if(idCar == -1){
			opened = 0;
		}
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

	//Create fifos
	mkfifo("fifoN",0660);
	mkfifo("fifoS",0660);
	mkfifo("fifoE",0660);
	mkfifo("fifoW",0660);

	//Park opens
	//	int fdN = open("fifoN", O_WRONLY || O_NONBLOCK);
	//	int fdS = open("fifoS", O_WRONLY || O_NONBLOCK);
	//	int fdE = open("fifoE", O_WRONLY || O_NONBLOCK);
	//	int fdW = open("fifoW", O_WRONLY || O_NONBLOCK);

	char NN = 'N';
	char SS = 'S';
	char EE = 'E';
	char WW = 'W';
	
	pLog = fopen("parque.log", "w");
	fprintf(pLog, "  t(ticks) ;   nLug    ;   id_viat ;   observs\n");


	pthread_t t;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);


	//creates thread 'controlador' N
	if((pthread_create(&t, &attr, &controlador, &NN)) != 0){
		printf("Error creating new 'controlador' thread\n");
		perror("Creating thread");
		exit(4);
	}

	//creates thread 'controlador' S
	if((pthread_create(&t, &attr, &controlador, &SS)) != 0){
		printf("Error creating new 'controlador' thread\n");
		perror("Creating thread");
		exit(4);
	}

	//creates thread 'controlador' E
	if((pthread_create(&t, &attr, &controlador, &EE)) != 0){
		printf("Error creating new 'controlador' thread\n");
		perror("Creating thread");
		exit(4);
	}

	//creates thread 'controlador' W
	if((pthread_create(&t, &attr, &controlador, &WW)) != 0){
		printf("Error creating new 'controlador' thread\n");
		perror("Creating thread");
		exit(4);
	}

	do{
		//saves current time
		time(&current);
	}while(difftime(current,start) < t_abertura); //compares current time with starting time and checks if it is time to close the park
	
	
	unlink("fifoN");
	unlink("fifoS");
	unlink("fifoE");
	unlink("fifoW");
	
	

	pthread_exit(NULL);

	return 0;

	//Park closes
	//Write -1 to fifos
}



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
