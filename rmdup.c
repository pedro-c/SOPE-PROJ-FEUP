#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {

  DIR *dirp;
  char *dir[];

  if (argc != 2) {
    fprintf(stderr, "Invalid Arguments");
    exit(1);
  }

  dir[1]=argv[1];

  int pid=fork();
  if(pid>0)//pai, compares and changes files that are equal
  {

  }
  else//child, reads the directories and adds the files to a text file
  {

    if ((dirp = opendir(dir[1])) == NULL) {
      fprintf(stderr, "NULL directory");
      exit(2);
    }

    chdir(dir[1]); //changes to the specified directory
    while((direntp == readdir(dirp)) != NULL) //reads all directory files
    {
      if (lstat(direntp->d_name, &stat_buf)==-1) {
        perror("lstat");
        exit(3);
      }
      if (S_ISREG(stat_buf.st_mode))
      str = "regular";
      else if (S_ISDIR(stat_buf.st_mode))
      str = "directory";


    }

  }






}
