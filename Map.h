#ifndef MAP_H
#define MAP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "DoubleLinkedList.h"

typedef struct MapNode{
    CacheNode* cache_node;
    struct MapNode* next;
} MapNode;

typedef struct Map{
    int arr_capacity;
    MapNode** backend_arr;
} Map;

MapNode* initialize_MapNode(CacheNode* cache_node);
void free_MapNode(MapNode* mapnode);
Map* initialize_Map(int cache_capacity);
void free_Map(Map* map);

void add_element(Map* map, int key, CacheNode* value_node);
void update_element(Map* map, int key, int value);
void del_element(Map* map, CacheNode* cache_node);
bool exists(Map* map, int key);
CacheNode* get(Map* map, int key);
#endif