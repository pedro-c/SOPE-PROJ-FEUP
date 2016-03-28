#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#define TRUE 1
#define FALSE 0
//ambos são ficheiros regulares;
//o ambos têm o mesmo nome;
//o ambos têm as mesmas permissões de acesso;
//o ambos têm o mesmo conteúdo.

void saveToFile(){

}

int compareContent(FILE *first, FILE *second)
{

  while(1){
    getline(first, line1);
    getline(second, line2);
    if(line1 != line2){
      return FALSE;
    }
    if(first.eof() && second.eof(){
      return TRUE;
    }
    if(first.eof() && !second.eof()){
      return FALSE;
    }
    if(!first.eof() && second.eof()){
      return FALSE;
    }

  }



}

void compareFiles(FILE *src){

  FILE *first, *second, *copy;
  char *fileName1[], *fileName2[], *dirName1[], *dirName2[], *size1[], *size2[], *permissions1[], *permissions2[];

  while(!src.eof())
  {
    getline(src, fileName1);
    getline(src, dirName1);
    getline(src, size1);
    getline(src, permissions1);
    copy = src;
    while(!copy.eof())
    {
      getline(src, fileName2);

      if(fileName1 != fileName2){
        continue;
      }

      getline(src, dirName2);

      if(permissoes1 != permissions2){
        continue;
      }

      getline(src, size2);
      if(size1 != size2){
        continue;
      }

      first = fopen(dirName1 + fileName1);
      second = fopen(dirName2 + fileName2);
      if(compareContent(first, second))
      {
        //update do ficheiro
      }
    }
  }
}


int main(int argc, char *argv[]) {
    DIR *dirp;
    char *dir[];
    struct dirent *direntp;
    char filenameTmp;
    struct stat stat_buf;
    int flag = TRUE;
    FILE *destination;

    if (argc != 2) {
      fprintf(stderr, "Invalid Arguments");
      exit(1);
    }

    if ((dirp = opendir(argv[1])) == NULL) {
      perror(argv[1]);
      exit(2);
    }

    dir = argv[1];


    int pid=fork();
    int pid2 = 1;

    if(pid>0) //pai, compares and changes files that are equal
    {
      wait();
      compareFiles(destination);
    }
    else //child, reads the directories and adds the files to a text file
    {
      while((direntp = readdir(dirp)) != NULL) //reads all directory files
      {
        if(pid > 0){
          flag = FALSE;
        }

        if(flag && pid2 = 0){
          closedir(dirp);
          dirp = opendir(filenameTmp);
          flag = FALSE;
        }

        if (lstat(direntp->d_name, &stat_buf)==-1){
          perror("lstat");
          exit(3);
        }

        if (S_ISREG(stat_buf.st_mode)){
          //falta sincronizar
            saveToFile(direntp, dirp);
        }

        else if (S_ISDIR(stat_buf.st_mode)){
          filenameTmp = stat_buf;
          flag = TRUE;
          pid2 = fork();
      }


      }
      closedir(dirp);

    }
  }
