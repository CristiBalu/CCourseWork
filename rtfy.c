#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define EMPTY_LIST           NULL
#define DELIM                 ",\n"

#define OUT_OF_MEMORY_ERR_MSG "Out of memory\n"
#define INPUT_FILE_ERR_MSG    "Could not open the input file\n"
#define ARG_NO_ERR_MSG        "You must enter three arguments\n"
#define INVALID_STN_ERR_MSG   "You entered invalid station names\n"
#define EMPTY_LIST_ERR_MSG    "Failed to remove from or inspect an empty list\n"

#define VALID_ARG_NO  4
#define TRUE          1
#define FALSE         0

struct Node{
  char        *value;
  struct Node *next;
};
typedef struct Node Node;

Node *push_back(Node *list, char *value);
Node *pop_front(Node *list);
char *head(Node *list);
int  findNode(Node *list, char *value);
Node *removeNode(Node *list, char *value);
void memoryCheck(void *pointer);
void exitWithMessage(char *message);
int  fileByteSize(FILE *file);
Node *neighbours(Node **adjacencies, int nStations, char *station);
int  getIndexByName(Node **adjacencies, int nStations, char *station);
void dijkstra(Node **adjacencies, int nStations, char *source, char *destination);

int main(int argc, char *argv[]){
  FILE *inputFile = NULL;
  char *input = NULL;                 // input file contents
  char *copy = NULL;
  char *station;                      // string for individual station names
  char *source, *destination;         // user input
  Node  **adjacencies;
  int   nStations = 0;
  int   i = 0;

  if(argc != VALID_ARG_NO){
    exitWithMessage(ARG_NO_ERR_MSG);
  }

  source = argv[2];
  destination = argv[3];

  if(!strlen(source) || !strlen(destination)){
    exitWithMessage(INVALID_STN_ERR_MSG);
  }

  if(!(inputFile = fopen(argv[1], "r"))){             // Try to open the input file
    exitWithMessage(INPUT_FILE_ERR_MSG);
  }

  input = (char*)malloc(fileByteSize(inputFile));   // Allocate space to fit the content
  memoryCheck(input);

  while(!feof(inputFile)){
    char c = fgetc(inputFile);

    if(c == '\n'){                        // Count the number of stations
      nStations++;
    }

    input[i++] = c;
  }

  adjacencies = (Node**) malloc(sizeof(Node*) * nStations);
  memoryCheck(adjacencies);

  for(i = 0; i < nStations; i++){       // Initialize the adjacency array with empty lists
    adjacencies[i] = EMPTY_LIST;
  }

  copy = strdup(input);                 // Extract stations from a copy of the input
  station = strtok(copy, DELIM);
  i = 0;

  while(station){
    if(strlen(station) > 0){
      char delimUsed = input[station-copy+strlen(station)];

      if(station[0] == ' '){
        station = station + 1;
      }

      adjacencies[i] = push_back(adjacencies[i], station);

      if(delimUsed == '\n' && i < nStations - 1){ // Increment if newline
        i++;
      }
    }
    station = strtok(NULL, DELIM);
  }

  dijkstra(adjacencies, nStations, source, destination);
  free(input);
  free(adjacencies);

  return 0;
}

/*
* Prints a message to stderr and exits the program with status 1.
*
* Arguments:
* message = string, the message to be printed
*/
void exitWithMessage(char *message){
  fprintf(stderr, message);
  exit(1);
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

/*
* Gives the neighbours of a specific node in an adjacency array.
*
* Arguments:
* adjacencies - array of lists, the adjacencies array
* nStations - the size of the array
* station - string, the value of the Node
*
* Returns a list of the station's neighbours or null if the node is not found
*/
Node *neighbours(Node **adjacencies, int nStations, char *station){
    int index = 0;

    for(index = 0; index < nStations; index++){
      if(!strcmp(adjacencies[index]->value, station)){
        return adjacencies[index]->next;
      }
    }

    return EMPTY_LIST;
}

/*
* Removes a node from a list
*
* Arguments:
* list - the list of nodes
* value - the value of the node whose first occurence will be removedNodes
*
* Returns a list simillar to the input list, but without the seeked node.
*/
Node *removeNode(Node *list, char *value){
  Node *temp = list;
  Node *prev = EMPTY_LIST;

  if(temp == NULL){
    return NULL;
  }

  if(temp->next == NULL){             // The list contains only a node
    if(!strcmp(temp->value,value)){   // If the node is the seeked one
      return NULL;
    }
    else{
      return list;
    }
  }

  while(temp != NULL && strcmp(temp->value, value)){ // Find the node
    prev = temp;
    temp=temp->next;
  }

  if(temp != EMPTY_LIST){
    if(prev == NULL){
      return pop_front(list);
    }
    prev->next = temp->next;
  }

  return list;
}

void dijkstra(Node **adjacencies, int nStations, char *source, char *destination){
    int distances[nStations];
    int prev[nStations];
    int removedNodes[nStations];
    int sourceIndex = getIndexByName(adjacencies, nStations, source);
    int i;
    Node *stations = EMPTY_LIST;

    for(i = 0; i < nStations; i++){
      distances[i] = INT_MAX;
      prev[i] = -1;
      removedNodes[i] = 0;
      stations = push_back(stations, adjacencies[i]->value);
    }

    distances[sourceIndex] = 0;

    while(stations != EMPTY_LIST)
    {
      int min = INT_MAX;
      int pos = -1;
      Node *adjacentNodes = EMPTY_LIST;

      for(i = 0; i < nStations; i++){
        if(!removedNodes[i] && distances[i] < min){
          min = distances[i];
          pos = i;
        }
      }

      if(!strcmp(adjacencies[pos]->value, destination)){
        int u = getIndexByName(adjacencies, nStations, destination);

        while(prev[u] != -1){
          printf("%s ", adjacencies[u]->value);
          u = prev[u];
        }
        printf("%s ", source);
      }

      stations = removeNode(stations, adjacencies[pos]->value);
      removedNodes[pos] = 1;
      adjacentNodes = neighbours(adjacencies, nStations, adjacencies[pos]->value);

      while(adjacentNodes != EMPTY_LIST){
        int temp = distances[pos] + 1;
        int neighbourIndex = getIndexByName(adjacencies, nStations, adjacentNodes->value);

        if(temp < distances[neighbourIndex]){
          distances[neighbourIndex] = temp;
          prev[neighbourIndex] = pos;
        }

        adjacentNodes = adjacentNodes->next;
      }
    }
}

/*
* Checks if a node exists in a list.
*
* Arguments:
* list - the list which will be searched
* value - the value to look for
*
* Returns 1 if a node with the seeked value exists in the list and 0 otherwise.
*/
int findNode(Node *list, char *value){
  Node *it = list;

  while(it != list){
    if(!strcmp(it->value, value)){
      return TRUE;
    }
  }

  return FALSE;
}

/*
* Looks for a station name in the adjacencies array and returns its index.
*/
int getIndexByName(Node **adjacencies, int nStations, char *station){
  int i;

  for(i = 0; i < nStations; i++){
    if(!strcmp(adjacencies[i]->value, station)){
      return i;
    }
  }

  return -1;
}

/*
* Inserts a node at end of a list
*
* Arguments:
* list - the list in which the node will be inserted
* value - string, the value of the new node
*
* Returns the same list with the new node inserted at the end
*/
Node *push_back(Node *list, char *value){
  Node *temp = EMPTY_LIST;
  Node *newNode = EMPTY_LIST;

  newNode = (Node*) malloc(sizeof(Node));
  memoryCheck(newNode);

  if(!newNode){
    exitWithMessage(OUT_OF_MEMORY_ERR_MSG);
  }

  newNode->value = value;
  newNode->next = EMPTY_LIST;

  if(list == EMPTY_LIST){
    return newNode;
  }

  temp = list;

  while(temp->next != EMPTY_LIST){ // Insert the node after the last node
    temp = temp->next;
  }

  temp->next = newNode;

  return list;
}

/*
* Removes the first node from a list
*
* Arguments:
* list - the list whose first node will be removed
*
* Returns the list without the first node
*/
Node *pop_front(Node *list){
  Node *temp = EMPTY_LIST;

  if(list == EMPTY_LIST){
    exitWithMessage(EMPTY_LIST_ERR_MSG);
  }

  if(list->next == EMPTY_LIST){
    return EMPTY_LIST;
  }

  temp = list->next;
  free(list);

  return temp;
}

char *head(Node *list){

  if(list == EMPTY_LIST){
    exitWithMessage(EMPTY_LIST_ERR_MSG);
  }

  return list->value;
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
