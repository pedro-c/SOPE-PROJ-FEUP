#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#define TRUE 1
#define FALSE 0
#define MAX 1024


/**
 * Function to create hard link between to files
 * @param path Path and name of the first file to link
 * @param path2 Path and name of the second file to link
 * @param hardLinksFilePath Path and name of the file to store the info on the hard links
 */
int createHardLink(char *path, char *path2, char* hardLinksFilePath){

		if(unlink(path2)==-1){
			perror("unlink");
			exit(4);
		}
		if(link(path, path2)==-1){
			perror("link");
			exit(4);
		}
		FILE *hardLinksFile;
		hardLinksFile = fopen(hardLinksFilePath, "a");

		sprintf(path, "%s", path);
		fprintf(hardLinksFile,path);
		sprintf(path, "-----> %s\n", path2);
		fprintf(hardLinksFile,path);
		fclose(hardLinksFile);




	return 0;
}

/**
 * Function to compare the content of two files
 * @param filePath1 Path and Name of the first file
 * @param filePath2 Path and Name of the second file
 * @param hardLinksFilePath Path to the file where the hardlinks are saved to be used when the files are equal
 */

int compareFileContent(char *filePath1, char *filePath2, char* hardLinksFilePath)
{
	FILE *file1, *file2;


	//used to compare files character by character
  char ch1, ch2;
	char* path = strtok(filePath1, "\n");
	char* path2 = strtok(filePath2, "\n");

	file1 = fopen(path, "r");

	if(file1 == NULL)
	{
		perror("Opening file1 to compare");
		exit(6);
	}

	file2 = fopen(path2, "r");

	if(file2 == NULL)
	{
		perror("Opening file2 to compare");
		exit(6);
	}


	//Compares char by char until the end of one file
	do{
		ch1 = getc(file1);
		ch2 = getc(file2);
	}while((ch1 != EOF) && (ch2 != EOF) && (ch1 == ch2));

	fclose(file1);
	fclose(file2);

	//if the characters remain the same, the loop ran through the entire two files, so they are identical
	if(ch1 == ch2){
			createHardLink(path, path2, hardLinksFilePath);
			return 1; //If they are identical, return 1 for true
	}


	//If they are not identical, return 0 for false
	return 0;
}

/**
 * Function to compare the content of two files
 * Uses "strtok" to divide the info in words
 * @param fileInfo1 All the information needed about the first file
 * @param fileInfo2 All the information needed about the second file
 * @param hardLinksFilePath Path to the file where the hardlinks are saved to be used when the files are equal
 */


int compareFiles(char *fileInfo1, char *fileInfo2, char *hardLinksFilePath)
{

//Gets all the information about the first file

	//Gets file Name
	char fileName1[MAX];
	char* token1 = strtok(fileInfo1, " ");

	sprintf(fileName1, "%s", token1);


	//Gets the date of the las t modification
	char date1[MAX];
	token1 = strtok(NULL, " ");
	sprintf(date1, "%s", token1);

	//Gets file size
	char size1[MAX];
	token1 = strtok(NULL, " ");
	sprintf(size1, "%s", token1);


	//Gets file's mode(permissions)
	char m1[MAX];
	token1 = strtok(NULL, " ");
	sprintf(m1, "%s", token1);

	//Gets path to file
	char path1[MAX];
	token1 = strtok(NULL, " ");
	sprintf(path1, "%s", token1);

//Gets all the information about the second file

	//Gets file Name
	char fileName2[MAX];
	char* token2 = strtok(fileInfo2, " ");
	sprintf(fileName2, "%s", token2);

	//Gets the date of last modification
	char date2[MAX];
	token2 = strtok(NULL, " ");
	sprintf(date2, "%s", token2);

	//Gets file size
	char size2[MAX];
	token2 = strtok(NULL, " ");
	sprintf(size2, "%s", token2);
	
	//Gets file's mode(permissions)
	char m2[MAX];
	token2 = strtok(NULL, " ");
	sprintf(m2, "%s", token2);

	//Gets path to file
	char path2[MAX];
	token2 = strtok(NULL, " ");
	sprintf(path2, "%s", token2);
	
//Compares the information of the two files
//Dates of the last modification are not used
	if(strcmp(fileName1, fileName2) != 0)
		return 0;
	if(strcmp(size1, size2) != 0)
		return 0;
	if(strcmp(m1, m2) != 0)
	  return 0;

	return compareFileContent(path1, path2, hardLinksFilePath);

}




/**
 * Function to create a the first new process
 * Executes "listdir" with the directory provided
 * @param filename Name and path of the new directory
 * @param home Directory where "listdir" is located
 */

int firstCreateProcess(char *fileName, char *home)
{
	pid_t pid;

  pid = fork();
  if(pid == 0)
  {
		char lsPath[MAX];
		sprintf(lsPath, "%s/listdir", home);
    execl(lsPath, "listdir", fileName, home, NULL);
    perror("execlp");
    exit(4);
  }
	if (pid > 0){
		wait(NULL);
		return 0;

	}
  return 1;
}

/**
 * Function to compare two strings
 */

int compare (const char *string, const char *string2)
{
  return ( string>string2 );
}


/**
 * Function to sort a file into alphabetical order
 * @param fileDPath Path and Name of the file to sort 
 */

int sortFile(char *fileDPath){
	
  pid_t pid;

//Creates a new process which will make a call to "sort"
  pid = fork();
  
  //If child, will call sort
	if(pid == 0)
  {
  		//Opens the intended file
		int file = open(fileDPath, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
		
		//Used to make the output of "sort" go to a file and not to the console
		dup2(file,1);
		
		
		close(file);
		
		//Executes "sort" from the shell to sort the file
		execlp( "sort", "sort", "files.txt", NULL);

  }
  //If father, will wait for child and then wrap up
  if (pid > 0){
	wait(NULL);
	return 0;
  }
  return 1;

}


/**
 * Main function
 * Expects a directory as the first and only argument
 */

int main(int argc, char *argv[]) {
	
	//Checks if the number of arguments is valid
	if (argc != 2) {
    		fprintf(stderr, "Invalid Arguments");
    		exit(1);
  	}

	//Creates memory space for the current working directory to be used later
	char cwd[MAX];
	
	//Creates memory space to store the path and name of the file which will contain all the information on the other files
	char fileDPath[MAX];
	
	//Creates memory space to store the path and name of the file which will contain all the information on the hardlinks
	char hardLinksFilePath[MAX];
	
	//Creates memory space for various buffers to be used later
	char buff[MAX];
	char buff2[MAX];
	char line[MAX];
	char line2[MAX];
	
	//Creates a counter for later usage
	int counter=0;

	//Gets current working directory (checks if successful)
	if(getcwd(cwd, sizeof(cwd)) == NULL)
	{
		perror("Getting CWD");
		exit(1);
	}
	
	//Stores name of the file to contain the info on other files
	sprintf(fileDPath, "%s/files.txt", cwd);
	
	//Stores name of the file to contain the info on hard links
	sprintf(hardLinksFilePath, "%s/hlinks.txt", argv[1]);

	//Create new empty file to store info
	FILE* fileD = fopen(fileDPath, "w");
	fclose(fileD);

	//Create new empty file to store Hard Links and prepares it
	FILE* hardLinksFile = fopen(hardLinksFilePath, "w");
	fprintf(hardLinksFile,"Hardlink ----> Original\n");
	fclose(hardLinksFile);

	//Creates a new process to start running through all the directories
	int pid = firstCreateProcess(argv[1], cwd);

	//
	sortFile(fileDPath);

  if(pid==0)
  {
		FILE *fileD;

		fileD = fopen (fileDPath, "r");

		while(fgets(buff, MAX, (FILE*)fileD) != NULL)
   	{
			FILE *fileD2;
			fileD2 = fopen (fileDPath, "r");
			int i=0;

			for(i=0; i< counter; i++){
				fgets(buff, MAX, (FILE*)fileD);
			}
			counter=0;

			while(fgets(buff2, MAX, (FILE*)fileD2) != NULL)
	   	{
				sprintf(line, "%s", buff);
				sprintf(line2, "%s", buff2);
					if (strcmp(line,line2) != 0){
						if(compareFiles(line2, line, hardLinksFilePath)==1){
							counter++;
						}
					}

	   	}
			fclose(fileD2);
   	}
   	fclose(fileD);




  }

  return 0;
}
