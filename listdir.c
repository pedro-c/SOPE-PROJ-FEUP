#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#define MAX 512


int createProcess(char *fileName, char *home)
{

  char lsPath[MAX];
  pid_t pid;

  pid = fork();
  if(pid == 0)
  {
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

//Ver como é guardado em string o time_t --> access time


int saveToFile(char *fileDPath, char *filePath, char *fileName, struct stat stats)
{
  FILE *file = fopen(fileDPath, "a");

	char fileInfo[MAX];
	sprintf(fileInfo, "%s %d %d %d %s\n", fileName, stats.st_mtime, stats.st_size, stats.st_mode, filePath);
	fprintf(file,fileInfo);
  fclose(file);
	return 0;
}


int main(int argc, char *argv[])
{
  DIR *dirp;
  struct dirent *direntp;
  struct stat stat_buf;
  char cwd[MAX];
  char fileDPath[MAX];



  if (argc != 3) {
    fprintf(stderr, "Invalid Arguments");
    exit(1);
  }



  if(getcwd(cwd, sizeof(cwd)) == NULL)
	{
		perror("Getting CWD in listdir");
		exit(1);
	}

  sprintf(fileDPath, "%s/files.txt", argv[2]);

  if ((dirp = opendir(argv[1])) == NULL) {
    perror(argv[1]);
    exit(2);
  }


  //Creates a Process for each new directory
  chdir(argv[1]);
  while((direntp = readdir(dirp)) != NULL)
  {
    if (lstat(direntp->d_name, &stat_buf)==-1){
      perror("lstat1");
      exit(3);
    }
    printf("diretorios: %s\n", direntp->d_name );
    if (strcmp(direntp->d_name,".") == 0)
        continue;
    else if (strcmp(direntp->d_name,"..") == 0)
        continue;
    // if it is a directory, launches new Process
    else if (S_ISDIR(stat_buf.st_mode)){
      char filePath[MAX];
      sprintf(filePath, "%s/%s", argv[1], direntp->d_name);
      createProcess(filePath, argv[2]);
    }
  }

  if ((dirp = opendir(argv[1])) == NULL) {
    perror(argv[1]);
    exit(2);
  }

  //Saves each file's information to a Text File
  chdir(argv[1]);
  while((direntp = readdir(dirp)) != NULL)
  {
    if (lstat(direntp->d_name, &stat_buf)==-1){
      perror("lstat2");
      exit(3);
    }
    printf("ficheiros: %s\n", direntp->d_name );
    if (strcmp(direntp->d_name,".") == 0)
        continue;
    else if (strcmp(direntp->d_name,"..") == 0)
        continue;
    else if (S_ISREG(stat_buf.st_mode)){
        printf("%s \n",direntp->d_name);
        char filePath[MAX];
        sprintf(filePath, "%s/%s", argv[1], direntp->d_name);
        saveToFile(fileDPath, filePath ,direntp->d_name, stat_buf);

    }
  }

  closedir(dirp);
  return 0;
}
