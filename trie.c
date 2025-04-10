#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "trie.h"
#include <ctype.h>

// Function to initialize the trie data structure
void init_trie(trie *t) {
    // Allocate memory for the root node
    t->root = (trie_node*)malloc(sizeof(trie_node));

    // Initialize all child nodes of the root to NULL
    for (int i = 0; i < MAX_CHILDREN; i++) {
        t->root->children[i] = NULL;
    }

    // Set the character for the root node to '\0'
    t->root->c = '\0';

    // Initialize the URL list for the root node as an empty list
    init_SLL(&(t->root->url_list));
    return;
}

// Function to update the URL list for a trie node
void update_url_list(trie_node *node, char *url) {
    // Append the URL to the node's URL list
    append(&(node->url_list), url);
    return;
}

// Function to map a character to an index in the children array
int get_index(char key) {
    // Handle lowercase letters (a-z)
    if (key >= 97 && key <= 122) {
        return (int)key - 97;
    }
    
    // Handle special characters for end of word and specific symbols
    if (key == '\0') return MAX_CHILDREN - 1;
    if (key == '\'') return MAX_CHILDREN - 2;
    if (key == '-') return MAX_CHILDREN - 3;
    if (key == '_') return MAX_CHILDREN - 4;

    // Return -1 for invalid characters
    return -1;
}

// Function to display the URL list for a trie node
void display_url_list(trie_node *node) {
    // If the node has no URL list, display a message
    if (!node->url_list) {
        printf("The list is empty.\n");
        return;
    }

    // Traverse and print each URL in the node's URL list
    sll_node *current = node->url_list;
    while (current) {
        printf("%s\n", current->url);
        current = current->next;
    }
}

// Function to insert a keyword and associated URL into the trie
void insert_trie(trie *t, char *keyword, char *url) {
    // Return if the trie or any required parameters are NULL or invalid
    if (!t || !t->root || !keyword || !url || *keyword == '\0') return;

    // Start at the root of the trie
    trie_node *current_node = t->root;

    // Traverse through the keyword and build the trie
    for (char *c = keyword; *c != '\0'; c++) {
        int index = get_index(*c);
        
        // Skip invalid characters
        if (index == -1) {
            return;
        }

        // Create a new node if the corresponding child doesn't exist
        if (!current_node->children[index]) {
            current_node->children[index] = create_node(*c);
        }

        // Move to the child node
        current_node = current_node->children[index];
    }

    // Update the URL list for the final node of the keyword
    update_url_list(current_node, url);
    return;
}

// Function to search for a keyword in the trie and display associated URLs
void search_trie(trie *t, char *keyword) {
    // Return if the trie or any required parameters are NULL or invalid
    if (!t || !t->root || !keyword || *keyword == '\0') {
        return; // Invalid input or empty keyword
    }
    
    // Convert the keyword to lowercase
    for (char *p = keyword; *p; p++) *p = tolower(*p);

    // Start at the root of the trie
    trie_node *current_node = t->root;

    // Traverse through the keyword in the trie
    for (char *c = keyword; *c != '\0'; c++) {
        int index = get_index(*c);

        // If the index is invalid or the child node doesn't exist
        if (index == -1 || !current_node->children[index]) {
            printf("Keyword not found\n"); // Keyword doesn't exist in the Trie
            return;
        }

        // Move to the child node
        current_node = current_node->children[index];
    }

    // If the node has a URL list, display the associated URLs
    if (current_node->url_list != NULL) {
        printf("Keyword found at the following URLs:\n");
        display_url_list(current_node);
    }

    return; // Keyword not found or no URLs associated
}

// Function to create a new trie node with a specific character
trie_node *create_node(char key) {
    // Allocate memory for a new trie node
    trie_node *node = (trie_node *)malloc(sizeof(trie_node));
    
    // Set the character for the node
    node->c = key;

    // Initialize all children to NULL
    for (int i = 0; i < MAX_CHILDREN; i++) {
        node->children[i] = NULL;
    }

    // Initialize the URL list for the new node as an empty list
    init_SLL(&(node->url_list));
    return node;
}

/*void display_trie_node(trie_node *node, int level) {
    if (!node) return;

    // Indent based on the level of the node
    for (int i = 0; i < level; i++) {
        printf("  ");
    }

    // Print the current node
    if (node->c == '\0') {
        printf("Root\n");
    } else {
        printf("'%c'", node->c);
        
        printf("\n");
    }

    // Recursively display all children
    for (int i = 0; i < MAX_CHILDREN; i++) {
        if (node->children[i]) {
            display_trie_node(node->children[i], level + 1);
        }
    }
}

// Function to display the entire Trie
void display_trie(trie *t) {
    if (!t || !t->root) {
        printf("Trie is empty.\n");
        return;
    }
    display_trie_node(t->root, 0);
}*/  

