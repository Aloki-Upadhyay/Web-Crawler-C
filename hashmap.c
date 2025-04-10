#include "hashmap.h"

// Hash function: converts the URL to a number using a simple polynomial rolling hash
unsigned long hash_function(const char *url) {
    unsigned long hash = 0; // Initialize hash value
    int p = 31;  // A small prime number for hash calculation
    unsigned long p_pow = 1; // Variable to hold powers of p
    
    // Iterate over each character in the URL
    while (*url) {
        // Calculate hash using polynomial rolling formula
        hash = (hash + (*url - 'a' + 1) * p_pow) % TABLE_SIZE;
        p_pow = (p_pow * p) % TABLE_SIZE; // Update power of p
        url++; // Move to the next character
    }
    return hash % TABLE_SIZE; // Ensure hash fits within the table size
}

// Initialize the hash map by setting all pointers in the table to NULL
void init_hashmap(HashMap *hashmap) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        hashmap->table[i] = NULL; // Initialize each bucket to NULL
    }
}

// Insert a URL into the hash map
void insert_url(HashMap *hashmap, const char *url) {
    unsigned long index = hash_function(url); // Get the hash index for the URL
    Node *new_node = (Node *)malloc(sizeof(Node)); // Allocate memory for a new node
    new_node->url = strdup(url);  // Duplicate the URL string and store it in the node
    new_node->next = hashmap->table[index];  // Point the new node to the current head of the list
    hashmap->table[index] = new_node;  // Insert the new node at the head of the list
}

// Search for a URL in the hash map, returns 1 if found, 0 if not found
int search_url(HashMap *hashmap, const char *url) {
    unsigned long index = hash_function(url); // Get the hash index for the URL
    Node *current = hashmap->table[index]; // Access the linked list at the hash index
    
    // Traverse the linked list to find the URL
    while (current) {
        if (strcmp(current->url, url) == 0) { // Compare the URL with the current node's URL
            return 1;  // URL found
        }
        current = current->next; // Move to the next node
    }
    return 0;  // URL not found in the hash map
}

// Free the memory allocated for the hash map
void free_hashmap(HashMap *hashmap) {
    // Iterate through each bucket in the hash table
    for (int i = 0; i < TABLE_SIZE; i++) {
        Node *current = hashmap->table[i]; // Get the head of the linked list
        while (current) {
            Node *temp = current; // Store the current node in a temporary pointer
            current = current->next; // Move to the next node
            free(temp->url);  // Free the memory allocated for the duplicated URL string
            free(temp);  // Free the memory allocated for the node
        }
    }
}



