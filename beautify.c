/**
* This program styles a paragraph so that it fits in a specified number of
* columns. The number of columns, the file containing the paragraph and the
* file to which the output will be written should be specified when the program
* is run, as arguments.
*
* e.g.: beautify -n 60 <input.txt> output.txt
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#define VALID_ARG_NO 5
#define BASE_TEN     10

int fileByteSize(FILE *file);
int wordsTotalLength(char **words, int left, int right);
int badness(char **words, int left, int right, int width);
void beautify(char **words, int wordCount, int width);

int main(int argc, char* argv[]){
  FILE *inputFile, *outputFile;
  char *inputFilePath, *outputFilePath;
  int  width;
  char *paragraph;
  int  length = 0, wordCount;
  char *word;
  char prec;
  char **words;

  if(argc != VALID_ARG_NO){
    printf("You must enter four arguments!");
    return EXIT_FAILURE;
  }

  inputFilePath  = argv[3];
  outputFilePath = argv[4];
  width = strtol(argv[2], NULL, BASE_TEN);  // Convert input to int. Returns 0 if invalid

  // If the width was not an integer or was negative
  if((width == 0 && strcmp(argv[2], "0")) || width < 0){
    printf("The second argument MUST be a positive integer");
    return EXIT_FAILURE;
  }

  if(!(inputFile = fopen(inputFilePath, "r"))){       // Try to open the input file
    printf("Could not open the input file!\n");
    return EXIT_FAILURE;                              // If failed, exit with error
  }

  paragraph = (char *)malloc(fileByteSize(inputFile));// Allocate space to fit the content
  wordCount = -1;                                     // Do not count eof as a word
  prec = ' ';

  while(!feof(inputFile)){
    char c = fgetc(inputFile);

    if(c != '\n')
    {
      paragraph[length++] = c;

      if(c != ' ' && prec == ' '){                   // Count the number of words
          wordCount++;
      }

      prec = c;
    }
    else{
      paragraph[length++] = ' ';
      wordCount++;
    }
  }

  words = (char **) malloc(wordCount * sizeof(char *));// Allocate space for array of words
  word = strtok(paragraph, " \n");
  wordCount = 0;
  words[wordCount] = word;

  while(word != NULL){                                // Build the array of words
    word = strtok(NULL, " ");

    if(word != NULL){
      wordCount++;
      words[wordCount] = (char *) malloc(sizeof(word));
      words[wordCount] = word;
    }
  }

  fclose(inputFile);

  if(!(outputFile = fopen(argv[4], "w"))){
    printf("Could not create the input file!\n");
    return EXIT_FAILURE;
  }

  beautify(words, wordCount, width);

  fclose(outputFile);
  return EXIT_SUCCESS;
}

/**
* Function which determines the size of a files' content in bytes.
*
* Arguments:
* file -- a pointer to the file which will be measured
*
* Return:
* size -- an int specifying the size of the file or 0 if the file is NULL
*/
int fileByteSize(FILE *file){
  int size;

  if(!file){
    return 0;                 // Stop if the file is NULL
  }

  fseek(file, 0, SEEK_END);   // Place the indicator at the end of the file
  size = ftell(file);
  fseek(file, 0, 0);          // Place the indicator back at the beginning

  return size;
}

int wordsTotalLength(char **words, int left, int right){
  int totalLength = 0;
  int i;

  for(i = left; i <= right; i++){
    totalLength += strlen(words[i]);
  }

  return totalLength;
}

int badness(char **words, int left, int right, int width){
  int lineLength = wordsTotalLength(words, left, right);
  if(lineLength > width){
    return INT_MAX;
  }
  else{
    return (width - lineLength) * (width - lineLength) * (width - lineLength);
  }
}

void beautify(char **words, int wordCount, int width){
  int cost[wordCount][wordCount];
  int mincost[wordCount];
  int result[wordCount];
  int i, j, k;

  for(i = 0; i < wordCount; i++){
    cost[i][i] = width - strlen(words[i]);

    for(j = i + 1; j < wordCount; j++){
      cost[i][j] = cost[i][j - 1] - strlen(words[j]) - 1;
    }
  }

  for(i=0; i < wordCount; i++){
      for(j = 0; j < wordCount ; j++){
          if(cost[i][j] < 0)
              cost[i][j] = INT_MAX;
          else
              cost[i][j] = cost[i][j] * cost[i][j];
      }
  }
  for(i = wordCount; i >= 0; i--){
      mincost[i] = cost[i][wordCount - 1];
      result[i] = wordCount;
      for(j = wordCount - 1; j > i; j--){
          if(cost[i][j-1] == INT_MAX)
              continue;
          if(mincost[i] > mincost[j] + cost[i][j-1]){
              mincost[i] = mincost[j]+cost[i][j-1];
              result[i] = j;
          }
      }
  }
  i = 0;
  do{
    int spacesToInsert;
    int *spacesPerWord;
    int cnt = 0;

    j = result[i];
    spacesToInsert = width - wordsTotalLength(words, i, j - 1);
    spacesPerWord = (int *) malloc(sizeof(int) * (j - i));

    for(k = 0; k < j - i; k++){
      spacesPerWord[k] = 0;
    }

    for(k = 0; k < spacesToInsert; k++){
      spacesPerWord[cnt] += 1;
      if(cnt == j - i - 1){cnt = 0;}
      else{cnt++;}
    }

    for(k = i; k < j; k++){
      int u;  
      printf("%s", words[k]);
      for(u = 0; u < spacesPerWord[k - i]; u++){
        printf(" ");
      }
    }

    printf("\n");
    i = j;
  } while(j < wordCount);
}
