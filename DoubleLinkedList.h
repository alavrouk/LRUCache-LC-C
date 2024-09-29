#ifndef DLL_H
#define DLL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct CacheNode {
    struct CacheNode* next;
    struct CacheNode* prev;
    int key;
    int value;
} CacheNode;

typedef struct DLLForLRU {
    int size;
    int capacity;
    CacheNode* head;
    CacheNode* tail;
} DLLForLRU;

typedef struct AddNewReturn {
    CacheNode* added;
    int removed_key;
    CacheNode* removed_node;
} AddNewReturn;

CacheNode* initialize_CacheNode(int key, int value);
DLLForLRU* initialize_DLLForLRU(int capacity);
AddNewReturn* initialize_AddNewReturn(CacheNode* added, int removed_key, CacheNode* removed_node);
AddNewReturn* add_new(DLLForLRU* dll, int key, int value);
void use_existing(DLLForLRU* dll, CacheNode* node);
void free_CacheNode(CacheNode* node);
void free_DLLForLRU(DLLForLRU* dll);
void free_AddNewReturn(AddNewReturn* returned);

#endif
