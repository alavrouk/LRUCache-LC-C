#ifndef LRU_H
#define LRU_H

#include <stdio.h>
#include <stdlib.h>

#include "DoubleLinkedList.h"
#include "Map.h"

typedef struct LRUCache {
    int capacity;
    Map* map;
    DLLForLRU* dll;
} LRUCache;

LRUCache* lRUCacheCreate(int capacity);
int lRUCacheGet(LRUCache* obj, int key);
void lRUCachePut(LRUCache* obj, int key, int value);
void lRUCacheFree(LRUCache* obj);

#endif LRU_H