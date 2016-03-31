#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
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

void saveToFile(char *files, char *fileName, struct stat stat){

    FILE *file=fopen(files, "a");
    if(file==NULL){
      perror("File opening");
      exit(5);
    }

    char finalString[MAX];

    //METER DADOS NA FINALSTRING
    fwrite(finalString, 1, MAX, file);



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

  char *files[] = argv[2];

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
      //falta sincronizar
        saveToFile(files, direntp->d_name, stat_buf);
    }
  }
  closedir(dirp);

  return 0;
}
