#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// cut last byte in outfile

///////////////////////////////////////////////////////////////////

int main ( int argc, char *argv[] )
{

  FILE *file_1;  // pointer on in  file
  FILE *file_2;  // pointer on out file

//  char name_1[80] = "trd.v11trd_may11_v11a.digi.par";
//  char name_2[80] = "trd.v11trd_may11_v11a.digi.par.cut";

  char *name_1;
  char *name_2;

  int data = 0;
  int buf  = 0;

/////////////////////////////////////////////////////////

//  printf("%d\n", argc);

  printf("%s by DE\n", argv[0]);

  if (argc < 3)
  {
    printf("usage: %s infile outfile\n", argv[0]);
    exit(1);
  }

//  printf("infile : %s\n", argv[1]);
//  printf("outfile: %s\n", argv[2]);

  name_1 = argv[1];
  name_2 = argv[2];

  // open files
  file_1 = fopen(name_1,"rb");
  if (file_1 == NULL)
  { 
    printf("error opening %s\n", name_1);
    exit(1);
  }
  file_2 = fopen(name_2,"wb");
  if (file_2 == NULL)
  { 
    printf("error opening %s\n", name_2);
    exit(1);
  }

  printf("\nstripping last byte of %s\n", argv[1]);
  printf("to %s\n", argv[2]);

  data = fgetc(file_1);
  while (!(feof(file_1)))
  {

    buf = data; 

    data = fgetc(file_1);

    if (!(feof(file_1)))  // if not last character, write to file 2
      fputc(buf,file_2);
  }

  printf("\n");

  fclose(file_2);
  fclose(file_1);

  char syscmd[80];

  printf("%s:\n", name_1);
  sprintf(syscmd, "tail -3 %s | hexdump -C", name_1);
  system(syscmd);

  printf("\n%s:\n", name_2);
  sprintf(syscmd, "tail -3 %s | hexdump -C", name_2);
  system(syscmd);

}
