#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#define TRUE 1
#define FALSE 0
#define MAX 256
//ambos são ficheiros regulares;
//o ambos têm o mesmo nome;
//o ambos têm as mesmas permissões de acesso;
//o ambos têm o mesmo conteúdo.



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

void compareFiles(char *files[]){

}



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




int main(int argc, char *argv[]) {


  if (argc != 2) {
    fprintf(stderr, "Invalid Arguments");
    exit(1);
  }

  char files[] = "\"To Save\" File";

  if(createProcess(argv[1], files) > 0)
  {
    wait(); // a completar
    compareFiles(files);
  }

  return 0;
}
