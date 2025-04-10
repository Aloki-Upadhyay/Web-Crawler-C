#include "sll.h"       
#include <stdio.h>     
#include <stdlib.h>    
#include <string.h>     

// Function to initialize a singly linked list (SLL)
void init_SLL(SLL *lp) {
    *lp = NULL; // Set the head pointer to NULL, indicating an empty list
    return;
}

// Function to append a new node to the end of the singly linked list
void append(SLL* lp, char *url) {
    
    // Allocate memory for a new node
    sll_node* nn = (sll_node*) malloc(sizeof(sll_node));
    
    // Allocate memory for the URL string and copy the provided URL into the node
    nn->url = (char*)malloc(strlen(url) + 1);
    strcpy(nn -> url, url);
    
    // Set the `next` pointer of the new node to NULL
    nn -> next = NULL;

    // If the list is not empty
    if(*lp) {
        sll_node* p = *lp; // Start from the head of the list
        
        // Traverse to the last node in the list
        while(p -> next)
            p = p -> next;
        
        // Link the new node to the end of the list
        p -> next = nn;
    }
    // If the list is empty
    else {
        *lp = nn; // Set the new node as the head of the list
    }
    
    return;
}

// Function to calculate and return the length of the singly linked list
int len(SLL l) {
    sll_node* p = l; // Start from the head of the list
    int i = 0;       // Initialize a counter to track the number of nodes

    // Traverse the list and increment the counter for each node
    while(p) {
        p = p -> next;
        i++;
    }

    return i; // Return the total count of nodes
}


