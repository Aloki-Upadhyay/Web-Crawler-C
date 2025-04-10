#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 100  // Define the size of the hash table; adjust based on expected data size

// Node structure for separate chaining
typedef struct Node {
    char *url;           // URL stored as a string
    struct Node *next;   // Pointer to the next node in the linked list (for collision handling)
} Node;

// Hashmap structure containing the hash table
typedef struct HashMap {
    Node *table[TABLE_SIZE];  // Array of pointers to `Node`, serving as the hash table
} HashMap;

unsigned long hash_function(const char *url); // Function to compute a hash value for a given URL
void init_hashmap(HashMap *hashmap); // Function to initialize the hash map by setting all buckets to NULL
void insert_url(HashMap *hashmap, const char *url); // Function to insert a URL into the hash map
int search_url(HashMap *hashmap, const char *url); // Function to search for a URL in the hash map, returning 1 if found and 0 otherwise
void free_hashmap(HashMap *hashmap); // Function to free all memory allocated for the hash map

