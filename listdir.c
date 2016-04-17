#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#define MAX 512

/**
 * Function to create a new process
 * Executes "listdir" with the directory provided
 * @param filename Name and path of the new directory
 * @param home Directory where "listdir" is located
 */
int createProcess(char *fileName, char *home)
{

	//Creates memory space to concatenate the path to "listdir" and its name
	char lsPath[MAX];

	//PID can be used to wait for a specific child
	pid_t pid;

	//Creates new process
	pid = fork();

	//If child --> will execute listdir with a new directory (will open fileName)
	if(pid == 0)
	{
		//Concatenates path and listdir's name
		sprintf(lsPath, "%s/listdir", home);

		execl(lsPath, "listdir", fileName, home, NULL);

		//If execl is successful, the code beneath it won't have any effect
		//That's way there is no "If" statement before "perror"
		perror("execlp");
		exit(4);
	}
	//If father, waits for child to end
	if (pid > 0){
		wait(NULL);
		return 0;

	}
	return 1;
}


/**
 * Function to save the informations of a file to another text file
 * @param fileDPath Path and name of the text file where information will be stored
 * @param filePath Path and name of the file which will have its information saved
 * @param fileName Name of the file which will have its information saved
 * @param stats Struct with various information on the file to be saved
 */

int saveToFile(char *fileDPath, char *filePath, char *fileName, struct stat stats)
{
	//Opens file in "append mode" because:
	//1. The file has already been previously created in "write mode"
	//2. Needs to append to the end of file because it will have information on many otehr files
	FILE *file = fopen(fileDPath, "a");

	//Checks if the file was successfully opened
	if(file == NULL)
	{
		perror("Opening file to store information");
		exit(6);
	}

	//Creates memory space for a single string containing all the information of a file
	char fileInfo[MAX];

	//Concatenates all the information of the file into the string previously created
	sprintf(fileInfo, "%s %d %d %d %s\n", fileName, stats.st_mtime, stats.st_size, stats.st_mode, filePath);

	//Prints the string to the destination file
	fprintf(file,fileInfo);
	fclose(file);


	return 0;
}


/**
 * Main function
 * Expects the new path of the new directory as the first parameter and the home path as the second
 */

int main(int argc, char *argv[])
{

	//Checks number of arguments
	if (argc != 3) {
		fprintf(stderr, "Invalid Arguments");
		exit(1);
	}


	//To open and use the directory
	DIR *dirp;

	//To run through the directory will using the information of each file/folder
	struct dirent *direntp;

	//To store the information of a file
	struct stat stat_buf;

	//Memory space to save the current working directory
	//This directory needs to be saved to later be used to get the full path to a file or folder
	char cwd[MAX];

	//The path and name of the file which will be used to store the information on other files
	char fileDPath[MAX];



	//Saves current working directory (and checks for successfulness of operation)
	if(getcwd(cwd, sizeof(cwd)) == NULL)
	{
		perror("Getting CWD in listdir");
		exit(2);
	}

	//Saves destination's file path and name
	sprintf(fileDPath, "%s/files.txt", argv[2]);


	//Changes to new directory
	if ((dirp = opendir(argv[1])) == NULL) {
		perror(argv[1]);
		exit(3);
	}
	chdir(argv[1]);

	//Cycle to run through all the contents of a directory and create a new process for each new folder(directory) inside it
	while((direntp = readdir(dirp)) != NULL)
	{
		//Gets the information on a file or directory (checks if successful)
		if (lstat(direntp->d_name, &stat_buf)==-1){
			perror("lstat1");
			exit(4);
		}

		//Ignores "." and ".." directories
		if (strcmp(direntp->d_name,".") == 0)
			continue;
		else if (strcmp(direntp->d_name,"..") == 0)
			continue;

		//If it is a directory other than "." and "..", launches new Process
		else if (S_ISDIR(stat_buf.st_mode)){
			//Reserving memory space for the concatenation of the path and the name of the new directory
			char filePath[MAX];

			//Concatenates name and path of new directory
			sprintf(filePath, "%s/%s", argv[1], direntp->d_name);

			//Launches process with new directory
			createProcess(filePath, argv[2]);
		}
	}

	//Resets directory
	if ((dirp = opendir(argv[1])) == NULL) {
		perror(argv[1]);
		exit(3);
	}
	chdir(argv[1]);

	//Cycle to run through all the contents of a directory and save the information of each file
	while((direntp = readdir(dirp)) != NULL)
	{
		//Gets the information on a file or directory (checks if successful)
		if (lstat(direntp->d_name, &stat_buf)==-1){
			perror("lstat2");
			exit(4);
		}

		//Ignores "." and ".." directories
		if (strcmp(direntp->d_name,".") == 0)
			continue;
		else if (strcmp(direntp->d_name,"..") == 0)
			continue;

		//If it is a regular file, save its information
		else if (S_ISREG(stat_buf.st_mode)){
			//Reserving memory space for the concatenation of the path and the name of the file
			char filePath[MAX];

			//Concatenates name and path of file
			sprintf(filePath, "%s/%s", argv[1], direntp->d_name);

			//Calls function to save the information of the file
			saveToFile(fileDPath, filePath ,direntp->d_name, stat_buf);
		}
	}

	//All done, closes directory
	closedir(dirp);
	return 0;
}
