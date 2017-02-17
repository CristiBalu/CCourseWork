#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_FILE_NAME "adjacencies.txt"
#define MAX_LINE_SIZE 4096

struct List{
  char        *value;
  struct List *next;
};

struct HashTable{
  int         size;
  struct List **buckets;
};

struct List       *mkList(char *value, struct List *next);
struct HashTable  *mkHashTable(int size);
int               hash(char* s);
void              addStation(char *s, struct HashTable *hashTable);

int main(){
  FILE              *adjacenciesFile;
  struct HashTable  *adjacencies;
  char              line[MAX_LINE_SIZE];

  if(!(adjacenciesFile = fopen(INPUT_FILE_NAME, "r"))){
    fprintf(stderr, "Could not open the input file!");
    exit(1);
  }

  while(fgets(line, sizeof(line), adjacenciesFile)){
    printf("%s", line);
  }

  return EXIT_SUCCESS;
}

struct List *mkList(char *value, struct List *next){
  struct List *newList = (struct List *) malloc (sizeof(struct List));

  if(!newList){
    fprintf(stderr, "mkList newList: Out of memory!\n");
    exit(1);
  }

  newList->value = value;
  newList->next = next;

  return newList;
}

struct HashTable *mkHashTable(int size){
  int i;
  struct HashTable *newHashTable = (struct HashTable*) malloc (sizeof(struct HashTable));

  if(newHashTable == NULL){
    exit(1);
  }

  newHashTable->size = size;
  newHashTable->buckets = (struct List**) malloc(sizeof(struct List) * size);

  for (i = 0; i < size; i++){
    newHashTable->buckets[i] = NULL;
  }

  return newHashTable;
}

void addStation(char *s, struct HashTable *hashTable){
  int h = hash(s);
  int pos = h % hashTable->size;

  hashTable->buckets[pos] = mkList(s, hashTable->buckets[pos]);
}

int hash(char* s){
	int i;
	int h = 0;

	for(i = 0; i < strlen(s); i++){
		h = h + (int) s[i];
	}

	return h;
}
