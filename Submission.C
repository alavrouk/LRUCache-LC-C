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

CacheNode* initialize_CacheNode(int key, int value)
{
    CacheNode* node = (CacheNode*) malloc(sizeof(CacheNode));
    if ( !node )
    {
        return NULL;
    }

    node->next = NULL;
    node->prev = NULL;
    node->key = key;
    node->value = value;

    return node;
}

DLLForLRU* initialize_DLLForLRU(int capacity)
{
    DLLForLRU* dll = (DLLForLRU*) malloc(sizeof(DLLForLRU));
    if (!dll)
    {
        printf("Memory Allocation for DLL Failed!\n");
        return NULL;
    }

    if ( capacity == 0 )
    {
        printf("You cannot have a 0 capacity DLL!\n");
        free(dll);
        return NULL;
    }

    dll->size = 0;
    dll->capacity = capacity;
    dll->head = NULL;
    dll->tail = NULL;

    return dll;
}

AddNewReturn* initialize_AddNewReturn(CacheNode* added, int removed_key, CacheNode* removed_node)
{
    AddNewReturn* returned = (AddNewReturn*) malloc(sizeof(AddNewReturn));
    if ( !returned )
    {
        printf("Memory Allocation for Returned struct Failed!\n");
        return NULL;
    }
    returned->added = added;
    returned->removed_key = removed_key;
    returned->removed_node = removed_node;
    return returned;
}

void free_CacheNode(CacheNode* node)
{
    node->next = NULL;
    node->prev = NULL;
    free(node);
    return;
}

void free_DLLForLRU(DLLForLRU* dll)
{
    CacheNode* curr = dll->head;
    dll->head = NULL;
    dll->tail = NULL;
    while ( curr )
    {
        CacheNode* prev = curr;
        curr = curr->next;
        free_CacheNode(prev);
    }
    free(dll);
    return;
}

void free_AddNewReturn(AddNewReturn* returned)
{
    returned->added = NULL;
    returned->removed_node = NULL;
    free(returned);
    return;
}

/* 
Append new node to the tail as most recently used.
The first step is to create a new cache node.
After that, there are two cases: 
1. If we are not at capacity, just append a new CacheNode and shift the tail pointer.
2. If we are at capacity, we need to remove the least recently used element then append.
 */
AddNewReturn* add_new(DLLForLRU* dll, int key, int value)
{
    // Build our new node
    CacheNode* node = initialize_CacheNode(key, value);
    if ( !node ) 
    {
        printf("Memory Allocation for CacheNode Failed!\n");
        return NULL;
    }

    CacheNode* removed_node = NULL;
    int removed = -1;

    // Edge case: empty list
    if (dll->size == 0)
    {
        dll->head = node;
        dll->tail = node;
        dll->size = 1;
        AddNewReturn* returned = initialize_AddNewReturn(node, -1, NULL);
        return returned;
    }

    // when the cache is full - remove current head
    if (dll->size >= dll->capacity)
    {
        if (dll->head == NULL)
        {
            printf("Error: dll->head is NULL when size >= capacity\n");
            free_CacheNode(node);
            return NULL;
        }
        CacheNode* old_head = dll->head;
        CacheNode* new_head = dll->head->next;
        removed = old_head->key;
        removed_node = old_head;
        if (new_head)
        {
            new_head->prev = NULL;
        }
        dll->head = new_head;
        if (dll->head == NULL)
        {
            // List is now empty, so update tail
            dll->tail = NULL;
        }
        dll->size -= 1;
        // Do not free old_head here
    }

    // update the new tail
    if (dll->tail != NULL)
    {
        dll->tail->next = node;
        node->prev = dll->tail;
        dll->tail = node;
    }
    else
    {
        // List is empty, so both head and tail are node
        dll->head = node;
        dll->tail = node;
    }
    dll->size += 1;

    // return our new node and removed key (-1 if we didn't remove anything)
    AddNewReturn* returned = initialize_AddNewReturn(node, removed, removed_node);
    return returned;
}

/*
This function uses something that was already in the DLL.
This entails disconnecting it from the list, and then adding it as a new tail.
*/
void use_existing(DLLForLRU* dll, CacheNode* node)
{
    // Edge case: our node is already the tail (most recently used)
    if ( dll->tail == node )
    {
        // do nothing :)
        return;
    }

    // Remove node from its current position
    if (node->prev)
        node->prev->next = node->next;
    if (node->next)
        node->next->prev = node->prev;

    if ( dll->head == node )
    {
        dll->head = node->next;
    }

    node->prev = dll->tail;
    node->next = NULL;
    if (dll->tail)
        dll->tail->next = node;
    dll->tail = node;
}

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

MapNode* initialize_MapNode(CacheNode* cache_node) 
{
    MapNode* mapnode = (MapNode*) malloc(sizeof(MapNode));
    if ( !mapnode )
    {
        printf("Memory Allocation for MapNode Failed!\n");
        return NULL;
    }
    mapnode->cache_node = cache_node;
    mapnode->next = NULL;
    return mapnode;
}

void free_MapNode(MapNode* mapnode)
{
    // Do not free the cache_node here to avoid double-free
    mapnode->cache_node = NULL;
    mapnode->next = NULL;
    free(mapnode);
    return;
}

Map* initialize_Map(int cache_capacity)
{
    Map* map = (Map*) malloc(sizeof(Map));
    if ( !map )
    {
        printf("Memory Allocation for Map Failed!\n");
        return NULL;
    }

    int new_arr_capacity = (int) (cache_capacity / 0.75);
    if (new_arr_capacity < 1)
        new_arr_capacity = 1;
    map->arr_capacity = new_arr_capacity;
    MapNode** backend_arr = (MapNode**) calloc(new_arr_capacity, sizeof(MapNode*));
    if ( !backend_arr )
    {
        printf("Memory Allocation for the backend array Failed!\n");
        free(map);
        return NULL;
    }
    map->backend_arr = backend_arr;
    return map;
}

void free_Map(Map* map)
{
    for (int i = 0; i < map->arr_capacity; ++i)
    {
        MapNode* curr_mapnode = map->backend_arr[i];
        while ( curr_mapnode )
        {
            MapNode* prev = curr_mapnode;
            curr_mapnode = curr_mapnode->next;
            free_MapNode(prev);
        }
    }
    free(map->backend_arr);
    free(map);
    return;
}

int hash_key(int key, int capacity)
{
    return (key * 2654435761U) % capacity;
}

/*
Add an element to the map. Assume that we know it does not exist.
*/
void add_element(Map* map, int key, CacheNode* value_node)
{
    MapNode* new_mapnode = initialize_MapNode(value_node);
    int hash = hash_key(key, map->arr_capacity);

    MapNode* location_ptr = map->backend_arr[hash];
    // If empty at loc
    if ( !location_ptr )
    {
        map->backend_arr[hash] = new_mapnode;
        return;
    }
    // otherwise append to end of LL
    MapNode* prev = NULL;
    while ( location_ptr )
    {
        if ( location_ptr->cache_node->key == key )
        {
            printf("During add element, found that key already exists\n");
            free_MapNode(new_mapnode);
            return;
        }
        prev = location_ptr;
        location_ptr = location_ptr->next;
    }
    prev->next = new_mapnode;
    return;
}

/*
Update an element in the map. Assume we know it exists in the map.
*/
void update_element(Map* map, int key, int value)
{
    int hash = hash_key(key, map->arr_capacity);
    MapNode* location_ptr = map->backend_arr[hash];

    while ( location_ptr )
    {
        if ( location_ptr->cache_node->key == key )
        {
            location_ptr->cache_node->value = value;
            return;
        }
        location_ptr = location_ptr->next;
    }

    printf("During update element, found that key does not exist\n");
    return;
} 

/* 
Delete an element from the map. Assume we know it exists in the map.
*/
void del_element(Map* map, CacheNode* cache_node)
{
    int hash = hash_key(cache_node->key, map->arr_capacity);
    MapNode* location_ptr = map->backend_arr[hash];

    if ( !location_ptr )
    {
        printf("During delete element, found that cache_node does not exist\n");
        return;
    }

    // Edge case: the head is the node we are looking to delete
    if ( location_ptr->cache_node == cache_node )
    {
        map->backend_arr[hash] = location_ptr->next;
        free_MapNode(location_ptr);
        return;
    }

    // General Case
    MapNode* prev = location_ptr;
    location_ptr = location_ptr->next;
    while ( location_ptr )
    {
        if ( location_ptr->cache_node == cache_node )
        {
            prev->next = location_ptr->next;
            free_MapNode(location_ptr);
            return;
        }
        prev = location_ptr;
        location_ptr = location_ptr->next;
    }

    printf("During delete element, found that cache_node does not exist\n");
    return;
}

/* 
Check whether a node exists in the map
*/
bool exists(Map* map, int key)
{
    int hash = hash_key(key, map->arr_capacity);
    MapNode* location_ptr = map->backend_arr[hash];

    while ( location_ptr )
    {
        if ( location_ptr->cache_node->key == key )
        {
            return true;
        }
        location_ptr = location_ptr->next;
    }

    return false;

}

/* 
Retrieve a node from the map
*/
CacheNode* get(Map* map, int key)
{
    int hash = hash_key(key, map->arr_capacity);
    MapNode* location_ptr = map->backend_arr[hash];

    while ( location_ptr )
    {
        if ( location_ptr->cache_node->key == key )
        {
            return location_ptr->cache_node;
        }
        location_ptr = location_ptr->next;
    }

    return NULL;

}

typedef struct LRUCache {
    int capacity;
    Map* map;
    DLLForLRU* dll;
} LRUCache;

LRUCache* lRUCacheCreate(int capacity);
int lRUCacheGet(LRUCache* obj, int key);
void lRUCachePut(LRUCache* obj, int key, int value);
void lRUCacheFree(LRUCache* obj);

LRUCache* lRUCacheCreate(int capacity) {
    LRUCache* lru_cache = (LRUCache*) malloc(sizeof(LRUCache));
    if ( !lru_cache )
    {
        printf("Memory Allocation for LRU cache Failed!\n");
        return NULL;
    }

    lru_cache->capacity = capacity;
    lru_cache->map = initialize_Map(capacity);
    lru_cache->dll = initialize_DLLForLRU(capacity);
    return lru_cache;
}

int lRUCacheGet(LRUCache* obj, int key) {
    if (obj->capacity == 0)
    {
        return -1;
    }

    if ( !exists(obj->map, key) )
    {
        return -1;
    }

    CacheNode* cache_node = get(obj->map, key);

    if ( !cache_node )
    {
        printf("Failed to get cache_node even though it should exist\n");
        return -1;
    }

    use_existing(obj->dll, cache_node);
    return cache_node->value;
}

void lRUCachePut(LRUCache* obj, int key, int value) {
    // Edge case: capacity is zero
    if (obj->capacity == 0)
    {
        return;
    }

    // Case where it exists in the map already
    if ( exists(obj->map, key) )
    {
        update_element(obj->map, key, value);
        CacheNode* cache_node = get(obj->map, key);
        use_existing(obj->dll, cache_node);
    }
    
    // Case where it does not exist in the map already
    else
    {
        AddNewReturn* returned = add_new(obj->dll, key, value);
        if (returned == NULL)
        {
            printf("Failed to add new node\n");
            return;
        }
        CacheNode* new_cache_node = returned->added;
        CacheNode* removed_node = returned->removed_node;

        if (removed_node != NULL)
        {
            del_element(obj->map, removed_node);
            free_CacheNode(removed_node);
        }
        add_element(obj->map, key, new_cache_node);
        free_AddNewReturn(returned);
    }
    return;
}

void lRUCacheFree(LRUCache* obj) {
    free_Map(obj->map);
    free_DLLForLRU(obj->dll);
    free(obj);
    return;
}

/**
 * Your LRUCache struct will be instantiated and called as such:
 * LRUCache* obj = lRUCacheCreate(capacity);
 * int param_1 = lRUCacheGet(obj, key);
 * lRUCachePut(obj, key, value);
 * lRUCacheFree(obj);
 */
