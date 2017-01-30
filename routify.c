#include <stdlib.h>
#include <stdio.h>

typedef struct Station{
  char *name;
  char **neighbours;
} Station;

int main(int argc, char *argv[]){
  FILE    *inputFile;
  char    *depart;
  char    *arrive;
  Station *stations;
  int     numberOfStations = 0;
  char    *line;

  if(argc != 3){
    printf("You must enter exactly two arguments");
    return EXIT_FAILURE;
  }

  depart = argv[1];
  arrive = argv[2];

  if((inputFile = fopen("adjacencies.txt", "r")) == NULL){  // Opening input file failed
    printf("Could not open the input file!\n");
    return EXIT_FAILURE;                                // Exit the program with an error
  }

  while(fgets(line, 2, inputFile)){
    numberOfStations++;
  }
  stations = (Station *) malloc(sizeof(Station) * numberOfStations);
  numberOfStations = 0;
  while(fgets(line, 2, inputFile)){
    stations[numberOfStations++] =
  }

  fclose(inputFile);

  return EXIT_SUCCESS;
}
