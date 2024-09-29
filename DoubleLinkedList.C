#include "DoubleLinkedList.h"

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