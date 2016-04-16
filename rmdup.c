#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>
#define TRUE 1
#define FALSE 0
#define MAX 1024

//ambos são ficheiros regulares;
//o ambos têm o mesmo nome;
//o ambos têm as mesmas permissões de acesso;
//o ambos têm o mesmo conteúdo.


int compareFileContent(char *filePath1, char *date1, char *filePath2, char *date2, char* hardLinksFilePath)
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

			printf("%s", path);

			if(date1 > date2){
				if(unlink(path)==-1){
					perror("unlink");
					exit(4);
				}
				if(link(path2, path)==-1){
					perror("link");
					exit(4);
				}
				FILE *hardLinksFile;
				hardLinksFile = fopen(hardLinksFilePath, "w");
				fprintf(hardLinksFile,path2);
				fclose(hardLinksFile);

			}else{
				if(unlink(path2)==-1){
					perror("unlink");
					exit(4);
				}
				if(link(path, path2)==-1){
					perror("link");
					exit(4);
				}
				FILE *hardLinksFile;
				hardLinksFile = fopen(hardLinksFilePath, "w");
				fprintf(hardLinksFile,path2);
				fclose(hardLinksFile);

			}



			return 1; //If they are identical, return 1 for true
	}


	//If they are not identical, return 0 for false
	return 0;
}

int compareFiles(char *fileInfo1, char *fileInfo2, char *hardLinksFilePath)
{

//FILEINFO1
	//get file name
	char fileName1[MAX];
	char* token1 = strtok(fileInfo1, " ");

	sprintf(fileName1, "%s", token1);


	//skip date
	char date1[MAX];
	token1 = strtok(NULL, " ");
	sprintf(date1, "%s", token1);

	//get size
	char size1[MAX];
	token1 = strtok(NULL, " ");
	sprintf(size1, "%s", token1);


	//get mode
	char m1[MAX];
	token1 = strtok(NULL, " ");
	sprintf(m1, "%s", token1);

	//get path
	char path1[MAX];
	token1 = strtok(NULL, " ");
	sprintf(path1, "%s", token1);

	//FILEINFO2
		//get file name
		char fileName2[MAX];
		char* token2 = strtok(fileInfo2, " ");
		sprintf(fileName2, "%s", token2);

		//skip date
		char date2[MAX];
		token2 = strtok(NULL, " ");
		sprintf(date2, "%s", token2);

		//get size
		char size2[MAX];
		token2 = strtok(NULL, " ");
		sprintf(size2, "%s", token2);

		//get mode
		char m2[MAX];
		token2 = strtok(NULL, " ");
		sprintf(m2, "%s", token2);

		//get path
		char path2[MAX];
		token2 = strtok(NULL, " ");
		sprintf(path2, "%s", token2);

		//for debuging
		//printf("%s %s %s %s\n", fileName1, size1, m1, path1 );
		//printf("%s %s %s %s\n", fileName2, size2, m2, path2 );


		if(strcmp(fileName1, fileName2) != 0)
			return 0;
		if(strcmp(size1, size2) != 0)
			return 0;
		if(strcmp(m1, m2) != 0)
		  return 0;


	return compareFileContent(path1, date1, path2, date2, hardLinksFilePath);

}




int createProcess(char *fileName, char *home)
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




int main(int argc, char *argv[]) {

	char cwd[MAX];
	char fileDPath[MAX];
	char hardLinksFilePath[MAX];
	char buff[MAX];
	char buff2[MAX];
	char line[MAX];
	char line2[MAX];
	//Gets current directory for later usage
	if(getcwd(cwd, sizeof(cwd)) == NULL)
	{
		perror("Getting CWD");
		exit(1);
	}
	sprintf(fileDPath, "%s/files.txt", cwd);
	sprintf(hardLinksFilePath, "%s/hlinks.txt", argv[1]);

	//Create new file to store info
	FILE* fileD = fopen(fileDPath, "w");
	fclose(fileD);

	//Create new file to store Hard Links
	FILE* hardLinksFile = fopen(hardLinksFilePath, "w");
	fclose(hardLinksFile);


  if (argc != 2) {
    fprintf(stderr, "Invalid Arguments");
    exit(1);
  }

	int pid = createProcess(argv[1], cwd);



  if(pid==0)
  {
		FILE *fileD;

		fileD = fopen (fileDPath, "r");

		while(fgets(buff, MAX, (FILE*)fileD) != NULL)
   	{
			FILE *fileD2;
			fileD2 = fopen (fileDPath, "r");
			while(fgets(buff2, MAX, (FILE*)fileD2) != NULL)
	   	{
				sprintf(line, "%s", buff);
				sprintf(line2, "%s", buff2);
				printf("1 : %s", line );
				printf("2 : %s", line2 );
					if (strcmp(line,line2) != 0){
						if(compareFiles(line, line2, hardLinksFilePath)==1){
							printf("Iguais!\n");
						}else{
							printf("Diferentes!\n");
						}
					}

	   	}
			fclose(fileD2);
   	}
   	fclose(fileD);




  }

  return 0;
}
