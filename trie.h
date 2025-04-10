#include <stdlib.h>
#include "sll.h"
#define MAX_CHILDREN 30

// Structure to represent a node in the trie
typedef struct trie_node {
    char c;                // Character stored at this node
    SLL url_list;          // Linked list of URLs associated with this node
    struct trie_node *children[MAX_CHILDREN];  // Array of pointers to child nodes (children for each character)
} trie_node;

// Structure to represent the trie (which contains the root node)
typedef struct trie {
    trie_node *root;  // Root node of the trie
} trie;

// Function prototypes

void update_url_list(trie_node *node, char *url); // Updates the URL list of a given trie node by appending the URL
void insert_trie(trie *t, char *word, char *url); // Inserts a word into the trie along with its associated URL
void search_trie(trie *t, char *keyword); // Searches for a keyword in the trie and displays the associated URLs
void init_trie(trie *t); // Initializes the trie by setting the root node and other required fields
trie_node *create_node(char key); // Creates and returns a new trie node for a given character
int get_index(char key); // Maps a character to an index in the children array
void display_url_list(trie_node *node); // Displays the URL list stored in a trie node











