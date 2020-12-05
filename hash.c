/***************************************************************
 * File Name    : hash.c
 * Description
 *      This is an implementation file for the open hash table.
 *
 ****************************************************************/

#include <stdlib.h>
#include "subc.h"
#include "subc.tab.h"

#define  HASH_TABLE_SIZE   101

typedef struct nlist {
    struct nlist *next;
    id *data;
} nlist;

static nlist *hashTable[HASH_TABLE_SIZE];


/// @brief Hash function: Divide the total sum of the characters in the 
/// @brief string by HASH_TABLE_SIZE
/// @param name Lexeme
/// @retval unsigned Hash value of the lexeme
unsigned hash(char *name) {
    unsigned int sum = 0;
    int i = 0;

    while (name[i] != '\0') {
        sum += (unsigned int)name[i];
        i = i + 1;
    }

    return sum % HASH_TABLE_SIZE;
}

/// @brief Add a new token to the hash table, or increase count if it already exists
///
/// @param tokenType integer indicating the token type
/// @param name Lexeme of the token
/// @param length Length of the lexeme
/// @retval Pointer to id struct containing information about the token
id *enter(int tokenType, char *name, int length) {
  // Do I need to copy the string or just use the address?
  
  // 1. Find the hash value
  // 2. Check if it exists -> Increase count
  // 3. If not, create nlist object, and add it to the tail
  // 4. Create id object for that token
  
  unsigned hash_value = hash(name);
  
  nlist *nlist_iter = hashTable[hash_value];
  nlist *prev_iter = hashTable[hash_value];
  id *item = NULL;
  while (nlist_iter != NULL) {
    item = nlist_iter->data;
    if (strcmp(item->name, name) == 0) {
      // Token already exists!
      // Increase count and return pointer to id
      item->count = (item->count) + 1;
      return item;
    }
    prev_iter = nlist_iter;
    nlist_iter = nlist_iter->next;
  }
  
  // Token not found
  // Add the token to the hashtable
  // The tail is pointed to by prev_iter 
  // (If the entry was empty, prev_iter = NULL)
  
  nlist *nlist_entry = malloc(sizeof(nlist));
  if (nlist_entry == NULL) {
    printf("Malloc error\n");
    exit(-1);
  }
  
  id *item_entry = malloc(sizeof(id));
  if (item_entry == NULL) {
    printf("Malloc error\n");
    exit(-1);
  }
  
  nlist_entry->next = NULL;
  nlist_entry->data = item_entry;
  
  item_entry->tokenType = tokenType;
  
  if (tokenType == ID) {
      // Need to copy the string
      item_entry->name = strdup(name);
      item_entry->count = 1;
  }
  else {
      // Count set to 0
      item_entry->name = strdup(name);
      item_entry->count = 0;
  }
  
  if (prev_iter == NULL) hashTable[hash_value] = nlist_entry;
  else prev_iter->next = nlist_entry;
  
  return item_entry;
}


/// @brief Print the hash table
/// @param 
/// @retval 
void ht_print() {
    int i;
    printf("Start of hash table\n");
    for (i=0; i<HASH_TABLE_SIZE; i++) {
        if (hashTable[i] != NULL) {
            nlist *nlist_iter = hashTable[i];
            printf("HashTable[%2d]: ", i);
            do {
                printf("%s, %d -> ", nlist_iter->data->name,
                                    nlist_iter->data->count);
                nlist_iter = nlist_iter->next;
            } while(nlist_iter != NULL);
            printf("END\n");
        }
    }
    printf("End of hash table\n");
}


/// @brief Return id* of id with the given name
/// @param char* name
/// @retval id* of the id with the given name
id *get_id_from_name(char *name) {
    int i;
    for (i=0; i<HASH_TABLE_SIZE; i++) {
        if (hashTable[i] != NULL) {
            nlist *nlist_iter = hashTable[i];
            do {
                if (strcmp(nlist_iter->data->name, name) == 0) {
                    return nlist_iter->data;
                }
                nlist_iter = nlist_iter->next;
            } while(nlist_iter != NULL);
        }
    }
    
    // No id with such name
    return NULL;
}
