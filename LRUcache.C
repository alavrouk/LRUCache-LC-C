#include "LRUCache.h"

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