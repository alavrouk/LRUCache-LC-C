#include "Map.h"

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