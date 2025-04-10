#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include <string.h>

// Initialize the queue by setting front and rear pointers to NULL
void qinit(queue *q) {
    q->front = NULL;
    q->rear = NULL;
}

// Add a new node to the rear of the queue
void enqueue(queue *q, char *url, int depth) {
    // Allocate memory for a new node
    node *new_node = (node *)malloc(sizeof(node));
    if (new_node == NULL) {  // Check if memory allocation failed
        printf("Memory allocation failed! Queue is full.\n");
        return;
    }
    
    // Allocate memory for the URL and copy it into the node
    new_node->url = (char *)malloc(strlen(url) + 1);
    new_node->depth = depth;  // Set the depth value for the node
    strcpy(new_node->url, url);
    new_node->next = NULL;  // This new node will be the last, so set `next` to NULL

    // If the queue is empty, set both front and rear to this new node
    if (q->rear == NULL) {
        q->front = q->rear = new_node;
    } else {
        // Otherwise, link the new node to the rear and update the rear pointer
        q->rear->next = new_node;
        q->rear = new_node;
    }
}

// Remove and return the front node from the queue
node *dequeue(queue *q) {
    // Check if the queue is empty
    if (isempty(q)) {
        printf("Empty queue\n");
        return NULL;  // Return NULL if there's nothing to dequeue
    }

    // Save the front node to a temporary pointer
    node *curr = q->front;
    // Move the front pointer to the next node
    q->front = q->front->next;

    // If the queue becomes empty after dequeue, set the rear pointer to NULL
    if (q->front == NULL) {
        q->rear = NULL;
    }

    return curr;  // Return the dequeued node
}

// Check if the queue is full (indirectly by testing memory allocation)
int isfull() {
    // Try to allocate memory for a new node
    node *curr = (node *)malloc(sizeof(node));
    if (curr == NULL) {  // If allocation fails, the queue is considered full
        return 1;
    }
    free(curr);  // Free the allocated memory immediately
    return 0;    // Return 0 to indicate the queue is not full
}

// Check if the queue is empty
int isempty(queue *q) {
    return (q->front == NULL);  // Queue is empty if the front pointer is NULL
}

// Print all URLs in the queue
void printqueue(queue *q) {
    // Check if the queue is empty
    if (isempty(q)) {
        printf("Queue is empty!\n");
        return;
    }

    // Iterate through the queue and print each URL
    node *temp = q->front;
    printf("Queue: ");
    while (temp != NULL) {
        printf("%s\n", temp->url);  // Print the URL of the current node
        temp = temp->next;  // Move to the next node
    }
    printf("\n");
}

