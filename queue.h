#include <stdio.h>
#include <string.h>

// Define the structure of a queue node
typedef struct node {
    char *url;            // Pointer to the URL string
    int depth;            // Depth level of the URL in a web crawling context
    struct node *next;    // Pointer to the next node in the queue
} node;

// Define the structure of the queue
typedef struct {
    node *front;          // Pointer to the front node of the queue
    node *rear;           // Pointer to the rear node of the queue
} queue;

// Function prototypes for queue operations
void qinit(queue *q); // Initialize the queue by setting both front and rear pointers to NULL
void enqueue(queue *q, char *url, int depth); // Add a new node with a URL and depth to the rear of the queue
node *dequeue(queue *q); // Remove and return the front node from the queue
int isfull(); // Check if the queue is full (based on system memory availability)
int isempty(queue *q); // Check if the queue is empty
void printqueue(queue *q); // Print all URLs in the queue along with their depth levels

