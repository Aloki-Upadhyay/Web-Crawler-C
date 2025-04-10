// Define a structure for a node in the singly linked list
typedef struct sll_node {
    char *url;              // A string to store the URL
    struct sll_node* next;  // A pointer to the next node in the list
} sll_node;  // Define the sll_node structure

// Define a pointer to the head node of the singly linked list
typedef sll_node* SLL;  // SLL is a pointer to the head node (first element) of the linked list

void init_SLL(SLL *lp); // Function to initialize the singly linked list by setting its head to NULL
void append(SLL* lp, char *url); // Function to append a new node with the specified URL to the end of the list
int len(SLL l); // Function to calculate and return the length (number of nodes) of the singly linked list


