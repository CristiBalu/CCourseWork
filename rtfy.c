#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define EMPTY_QUEUE  NULL
#define VALID_ARG_NO 4
#define TRUE         1
#define FALSE        0

struct Node{
  char        *value;
  struct Node *next;
};
typedef struct Node Node;

Node *enqueue(Node *queue, char *value);
Node *dequeue(Node *queue);
char *head(Node *queue);
int  findNode(Node *queue, char *value);
Node *removeNode(Node *list, char *value);
void printQueue(Node *queue);
void memoryCheck(void *pointer);
int  fileByteSize(FILE *file);
Node *neighbours(Node **adjacencies, int nStations, char *station);
int  getIndexByName(Node **adjacencies, int nStations, char *station);
void dijkstra(Node **adjacencies, int nStations, char *source, char *destination);

int main(int argc, char *argv[]){
  FILE *inputFile = NULL;
  char *input = NULL;
  char *copy = NULL;
  char *station;
  char *source, *destination;
  Node  **adjacencies;
  int   nStations = 0;
  int   i = 0;

  if(argc != VALID_ARG_NO){
    fprintf(stderr, "You must enter four arguments!");
    exit(1);
  }

  source = argv[2];
  destination = argv[3];

  if(!(inputFile = fopen(argv[1], "r"))){             // Try to open the input file
    fprintf(stderr, "Could not open the input file!\n");
    exit(1);                                         // If failed, exit with error
  }

  input = (char*)malloc(fileByteSize(inputFile));   // Allocate space to fit the content
  memoryCheck(input);

  while(!feof(inputFile)){
    char c = fgetc(inputFile);

    if(c == '\n'){
      nStations++;
    }

    input[i++] = c;
  }

  adjacencies = (Node**) malloc(sizeof(Node*) * nStations);
  memoryCheck(adjacencies);

  for(i = 0; i < nStations; i++){       // Initialize the adjacency array with empty lists
    adjacencies[i] = EMPTY_QUEUE;
  }

  copy = strdup(input);
  station = strtok(copy, ",\n");
  i = 0;

  while(station){
    if(strlen(station) > 0){
      char delimUsed = input[station-copy+strlen(station)];

      if(station[0] == ' '){
        station = station + 1;
      }

      adjacencies[i] = enqueue(adjacencies[i], station);

      if(delimUsed == '\n' && i < nStations - 1){
        i++;
      }
    }
    station = strtok(NULL, ",\n");
  }

  dijkstra(adjacencies, nStations, argv[2], argv[3]);
  free(input);
  free(adjacencies);

  return 0;
}

void memoryCheck(void *pointer){
  if(pointer == NULL){
    fprintf(stderr, "Out of memory!\n");
    exit(1);
  }
}

Node *neighbours(Node **adjacencies, int nStations, char *station){
    int index = 0;

    for(index = 0; index < nStations; index++){
      if(!strcmp(adjacencies[index]->value, station)){
        return adjacencies[index]->next;
      }
    }

    return EMPTY_QUEUE;
}

Node *removeNode(Node *list, char *value){
  Node *temp = list;
  Node *prev = EMPTY_QUEUE;

  if(temp == NULL) return NULL;
  if(temp->next == NULL){
    if(!strcmp(temp->value,value)){
      return NULL;
    }
    else{
      return list;
    }
  }

  while(temp != NULL && strcmp(temp->value, value)){
    prev = temp;
    temp=temp->next;
  }

  if(temp != EMPTY_QUEUE){
    if(prev == NULL){
      return dequeue(list);
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
    Node *stations = EMPTY_QUEUE;

    for(i = 0; i < nStations; i++){
      distances[i] = INT_MAX;
      prev[i] = -1;
      removedNodes[i] = 0;
      stations = enqueue(stations, adjacencies[i]->value);
    }

    distances[sourceIndex] = 0;

    while(stations != EMPTY_QUEUE)
    {
      int min = INT_MAX;
      int pos = -1;
      Node *adjacentNodes = EMPTY_QUEUE;

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

      while(adjacentNodes != EMPTY_QUEUE){
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

int findNode(Node *queue, char *value){
  Node *it = queue;

  while(it != queue){
    if(!strcmp(it->value, value)){
      return TRUE;
    }
  }

  return FALSE;
}

int getIndexByName(Node **adjacencies, int nStations, char *station){
  int i;

  for(i = 0; i < nStations; i++){
    if(!strcmp(adjacencies[i]->value, station)){
      return i;
    }
  }

  return -1;
}

Node *enqueue(Node *queue, char *value){
  Node *temp = EMPTY_QUEUE;
  Node *newNode = EMPTY_QUEUE;

  newNode = (Node*) malloc(sizeof(Node));
  memoryCheck(newNode);

  if(!newNode){
    fprintf(stderr, "Out of memory!\n");
    exit(1);
  }

  newNode->value = value;
  newNode->next = EMPTY_QUEUE;

  if(queue == EMPTY_QUEUE){
    return newNode;
  }

  temp = queue;

  while(temp->next != EMPTY_QUEUE){
    temp = temp->next;
  }

  temp->next = newNode;

  return queue;
}

Node *dequeue(Node *queue){
  Node *temp = EMPTY_QUEUE;

  if(queue == EMPTY_QUEUE){
    fprintf(stderr, "dequeue: Trying to dequeue from an empty queue");
    exit(1);
  }

  if(queue->next == EMPTY_QUEUE){
    return EMPTY_QUEUE;
  }

  temp = queue->next;
  free(queue);

  return temp;
}

char *head(Node *queue){

  if(queue == EMPTY_QUEUE){
    fprintf(stderr, "head: Trying to inspect an empty queue");
    exit(1);
  }

  return queue->value;
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

void printQueue(Node *queue){
  Node *temp = queue;

  while(temp != EMPTY_QUEUE){
    printf("%s", temp->value);
    temp = temp->next;
  }

  printf("\n");
}
