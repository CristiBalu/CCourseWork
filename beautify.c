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

#define ARG_NO_ERR_MSG        "You must enter four arguments\n"
#define INV_WIDTH_ERR_MSG     "The second argument must be a positive integer\n"
#define IN_FILE_ERR_MSG       "Could not open the input file\n"
#define OUT_FILE_ERR_MSG      "Could not create the output file\n"
#define OUT_OF_MEMORY_ERR_MSG "Out of memory\n"
#define WORD_LEN_ERR_MSG      "The length of a word is bigger than the line width\n"

#define VALID_ARG_NO 5
#define BASE_TEN     10
#define DELIM        " "
#define INV_LEN      -1

int fileByteSize(FILE *file);
int wordsTotalLength(char **words, int left, int right);
void beautify(char **words, int wordCount, int width, FILE *outputFile);
void exitWithMessage(char *message);
void memoryCheck(void *pointer);

int main(int argc, char* argv[]){
  FILE *inputFile, *outputFile;
  int  width;
  char *paragraph;
  int  length = 0, wordCount;
  char *word;
  char prec;
  char **words;

  if(argc != VALID_ARG_NO){
    exitWithMessage(ARG_NO_ERR_MSG);
  }

  width = strtol(argv[2], NULL, BASE_TEN);  // Convert input to int. Returns 0 if invalid

  // If the width was not an integer or was negative
  if(((width == 0 && !strcmp(argv[2], "0"))) || width < 0){
    exitWithMessage(INV_WIDTH_ERR_MSG);
  }

  if(!(inputFile = fopen(argv[3], "r"))){             // Try to open the input file
    exitWithMessage(IN_FILE_ERR_MSG);
  }

  paragraph = (char *)malloc(fileByteSize(inputFile));// Allocate space to fit the content
  memoryCheck(paragraph);
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
  memoryCheck(words);
  word = strtok(paragraph, DELIM);
  wordCount = 0;
  words[wordCount] = word;

  while(word != NULL){                                // Build the array of words
    word = strtok(NULL, DELIM);

    if(word != NULL){
      if(strlen(word) > width){
        exitWithMessage(WORD_LEN_ERR_MSG);
      }

      wordCount++;
      words[wordCount] = (char *) malloc(sizeof(word));
      memoryCheck(words[wordCount]);
      words[wordCount] = word;
    }
  }

  fclose(inputFile);

  if(!(outputFile = fopen(argv[4], "w"))){
    exitWithMessage(OUT_FILE_ERR_MSG);
  }

  beautify(words, wordCount, width, outputFile);
  fclose(outputFile);

  return EXIT_SUCCESS;
}


/*
* Checks if a pointer is null and exits with an error message if so.
*
* Arguments:
* pointer - the pointer to be checked
*/
void memoryCheck(void *pointer){
  if(pointer == NULL){
    exitWithMessage(OUT_OF_MEMORY_ERR_MSG);
  }
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

/*
* Calculates the length of a sequence of words
*
* Arguments:
* words - the array of words
* left - the left bound of the desired sequence
* right - the right bound of the desired sequence
*
* Returns the length of the sequence or -1 if it could not be calculated
*/
int wordsTotalLength(char **words, int left, int right){
  int totalLength = 0;
  int i;

  if(left > right){
    return INV_LEN;
  }

  for(i = left; i <= right; i++){
    totalLength += strlen(words[i]);
  }

  return totalLength;
}

/*
* Uses a dynamic programming approach to find out which words go on which lines, given
* the width of the line. Then it inserts spaces on a round robin basis in between the
* words of each line.
*/
void beautify(char **words, int wordCount, int width, FILE *outputFile){
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

  // Calculate the words for each line so that the amount of spaces is minimum
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
  // Insert spaces in between the word to match the line length
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

    // Print the beautified paragraph to the output file
    for(k = i; k < j; k++){
      int u;
      fprintf(outputFile, "%s", words[k]);
      for(u = 0; u < spacesPerWord[k - i]; u++){
        fprintf(outputFile, " ");
      }
    }

    fprintf(outputFile, "\n");
    i = j;
  } while(j < wordCount);
}

void exitWithMessage(char *message){
  fprintf(stderr, message);
  exit(1);
}
