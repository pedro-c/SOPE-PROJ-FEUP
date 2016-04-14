#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#define MAX 512


int createProcess(char *fileName, char *files)
{
  int pid = fork();
  if(pid == 0)
  {
    execlp("listdir", "listdir", fileName, files, NULL);
    perror("execlp");
    exit(4);
  }
  return pid;
}

//Ver como é guardado em string o time_t --> access time


int saveToFile(char *fileDPath, char *filePath, char *fileName, struct stat stats)
{

	char *time = (char*)(stats.st_atime);
	char *size = (char*)(stats.st_size);
	char *mode = (char*)(stats.st_mode);

	char fileInfo[MAX];

	strcat(fileInfo, fileName);
	strcat(fileInfo, " ");
	strcat(fileInfo, time);
	strcat(fileInfo, " ");
	strcat(fileInfo, size);
	strcat(fileInfo, " ");
	strcat(fileInfo, mode);
	strcat(fileInfo, " ");
	strcat(fileInfo, filePath);
	strcat(fileInfo, "\n");


	//Put data into fileD
	FILE* fileD = fopen(fileDPath, "a");
	if(fileD == NULL)
	{
		perror("FileD opening");
		exit(5);
	}


	fprintf(fileD, fileInfo);
	fclose(fileD);

	return 0;
}


int main(int argc, char *argv[])
{
  DIR *dirp;
  struct dirent *direntp;
  char filenameTmp;
  struct stat stat_buf;

  if (argc != 3) {
    fprintf(stderr, "Invalid Arguments");
    exit(1);
  }

  char *files = argv[2];

  if ((dirp = opendir(argv[1])) == NULL) {
    perror(argv[1]);
    exit(2);
  }


  //Creates a Process for each new directory
  while((direntp = readdir(dirp)) != NULL)
  {
    if (lstat(direntp->d_name, &stat_buf)==-1){
      perror("lstat");
      exit(3);
    }

    // if it is a directory, launches new Process
    if (S_ISDIR(stat_buf.st_mode)){
      createProcess(direntp->d_name, files);
    }
  }
  rewinddir(dirp);

  //Saves each file's information to a Text File
  while((direntp = readdir(dirp)) != NULL)
  {
    if (lstat(direntp->d_name, &stat_buf)==-1){
      perror("lstat");
      exit(3);
    }

    if (S_ISREG(stat_buf.st_mode)){
        printf("%-25s \n",direntp->d_name);
        //saveToFile(files, direntp->d_name, stat_buf);
    }
  }
  closedir(dirp);

  return 0;
}
