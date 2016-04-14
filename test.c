

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

#define MAX 256

//Ver como é guardado em string o time_t --> access time


int saveToFile(FILE * fileD, char *filePath, char *fileName, struct stat stats)
{


	char fileInfo[MAX];



	sprintf(fileInfo, "%s %d %d %d %s\n", fileName, stats.st_mtime, stats.st_size, stats.st_mode, filePath);

	fprintf(fileD,fileInfo);


	return 0;
}

int compareFileContent(char *filePath1, char *filePath2)
{
	FILE *file1, *file2;

	//used to compare files character by character
	int ch1, ch2;

	file1 = fopen(filePath1, "r");

	if(file1 == NULL)
	{
		perror("Opening file1 to compare");
		exit(6);
	}

	file2 = fopen(filePath2, "r");

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

	if(ch1 == ch2) //if the characters remain the same, the loop ran through the entire two files, so they are identical
		return 1; //If they are identical, return 1 for true

	//If they are not identical, return 0 for false
	return 0;
}

int compareFiles(char *fileInfo1, char *fileInfo2)
{

	//Reads first word, i.e., fileName

	char* token1 = strtok(fileInfo1, " ");
	char* token2 = strtok(fileInfo2, " ");

	char fileName1[MAX];
	char fileName2[MAX];
	fileName1[0] = '\0';
	fileName2[0] = '\0';

	strcat(fileName1, token1);
	strcat(fileName2, token2);

	//compares files' names. On different file names, return 0 for false
	//strcmp returns 0 if the strings are equal!

	if(strcmp(fileName1, fileName2))
		return 0;



	//Skips second word, i.e., last access time

	token1 = strtok(NULL, " ");
	token2 = strtok(NULL, " ");


	char size1[MAX];
	char size2[MAX];
	size1[0] = '\0';
	size2[0] = '\0';

	//Reads third word, i.e., file size

	token1 = strtok(NULL, " ");
	token2 = strtok(NULL, " ");

	strcat(size1, token1);
	strcat(size2, token2);

	//compares files' size

	if(strcmp(size1, size2))
		return 0;



	char m1[MAX];
	char m2[MAX];
	m1[0] = '\0';
	m2[0] = '\0';

	//Reads forth word, i.e., mode

	token1 = strtok(NULL, " ");
	token2 = strtok(NULL, " ");

	strcat(m1, token1);
	strcat(m2, token2);

	//compares files' size

	if(strcmp(m1, m2))
		return 0;

	char path1[MAX];
	char path2[MAX];
	path1[0] = '\0';
	path2[0] = '\0';

	token1 = strtok(NULL, " ");
	token2 = strtok(NULL, " ");

	strcat(path1, token1);
	strcat(path2, token2);


	return compareFileContent(path1, path2);

}


int main()
{
  struct dirent *direntp1;
  struct dirent *direntp2;
  struct stat stat_buf1;
  struct stat stat_buf2;


  FILE * fileD = fopen("/home/pedro/Desktop/Test/dest.txt", "w");
  fclose(fileD);

  DIR *dirp1;
  DIR *dirp2;

  dirp1 = opendir("/home/pedro/Desktop/Test/test1/test3");
  dirp2 = opendir("/home/pedro/Desktop/Test/test2");

  direntp1 = readdir(dirp1);
  direntp2 = readdir(dirp2);

  lstat(direntp1->d_name, &stat_buf1);
  lstat(direntp2->d_name, &stat_buf2);


  fileD = fopen("/home/pedro/Desktop/Test/dest.txt", "a");

  saveToFile(fileD, "/home/pedro/Desktop/Test/test1/test3/doc.txt", "doc.txt", stat_buf1);
  saveToFile(fileD, "/home/pedro/Desktop/Test/test1/test2/doc.txt", "doc.txt", stat_buf1);


  fclose(fileD);

	if(compareFiles("doc.txt 1460629843 4096 16893 /home/pedro/Desktop/Test/test1/test3/doc.txt\0", "doc.txt 1460629843 4096 16893 /home/pedro/Desktop/Test/test1/test2/doc.txt\0"))
	{
		printf("Iguais\n");
	}
	else
	{
		printf("Diferentes\n");
	}

	return 0;
}
