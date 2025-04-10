#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <ctype.h>
#include "queue.h"
#include "trie.h"
#include "hashmap.h"

// Data structure to store the response from a URL
typedef struct {
    char *data;   // Pointer to store response data
    size_t size;  // Size of the data
} ResponseData;

// Callback function to handle the response data
size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *respdata) {
    // Calculate the size of the current chunk of data
    size_t total_size = size * nmemb;

    // Cast the response data pointer
    ResponseData *respdata2 = (ResponseData *)respdata;

    // Reallocate memory to store the new data
    char *new_data = realloc(respdata2->data, respdata2->size + total_size + 1);
    if (new_data == NULL) {
        return CURLE_WRITE_ERROR;  // Return error if memory allocation fails
    }

    respdata2->data = new_data;  // Update the pointer to the reallocated memory
    memcpy(&(respdata2->data[respdata2->size]), contents, total_size);  // Copy new data into memory
    respdata2->size += total_size;  // Update the total size
    respdata2->data[respdata2->size] = '\0';  // Null-terminate the data

    return total_size;  // Return the size of processed data
}

// Function to check if a URL is valid
int is_valid_URL(char *url) {
    // Invalid if it starts with '#' or contains "javascript:" or "mailto:"
    if (url[0] == '#' || strstr(url, "javascript:") || strstr(url, "mailto:")) {
        return 0;
    }
    return 1;  // Valid URL otherwise
}

// Function to extract the base URL (protocol + domain) from a given URL
void extract_base_url(char *url, char *base_url, size_t max_length) {
    const char *scheme_end = strstr(url, "://");  // Find "://"
    if (scheme_end) {
        scheme_end += 3;  // Move past "://"
        const char *path_start = strchr(scheme_end, '/');  // Find the start of the path
        if (path_start) {
            size_t base_length = path_start - url;  // Calculate length of the base URL
            strncpy(base_url, url, (base_length < max_length) ? base_length : max_length - 1);
            base_url[(base_length < max_length) ? base_length : max_length - 1] = '\0';
        } else {
            strncpy(base_url, url, max_length - 1);  // Copy the whole URL if no path is found
            base_url[max_length - 1] = '\0';
        }
    } else {
        strncpy(base_url, url, max_length - 1);  // If no "://" is found, copy the URL
        base_url[max_length - 1] = '\0';
    }
}

// Function to find and process all <a> tags in the HTML
void find_links(char *url, char *html, HashMap *hashmap, queue *q, int depth) {
    char *a_tag_start = "<a ";    // Start of an <a> tag
    char *href_start = "href=\""; // Start of an href attribute
    char *href_end = "\"";        // End of an href attribute

    char base_url[256];
    extract_base_url(url, base_url, sizeof(base_url));  // Extract base URL for resolving relative links

    char *pos = html;  // Pointer to traverse HTML

    while ((pos = strstr(pos, a_tag_start)) != NULL) {
        // Find href attribute within the <a> tag
        char *href_pos = strstr(pos, href_start);

        if (href_pos) {
            href_pos += strlen(href_start);  // Move to the start of the URL

            // Find the end of the href value
            char *href_close = strstr(href_pos, href_end);

            if (href_close) {
                size_t link_length = href_close - href_pos;

                // Allocate memory to store the extracted URL
                char *link = (char *)malloc(link_length + 1);
                strncpy(link, href_pos, link_length);
                link[link_length] = '\0';  // Null-terminate the URL

                if (is_valid_URL(link)) {
                    // Handle relative URLs by prepending the base URL
                    if (link[0] == '/') {
                        char *new_link = (char *)malloc(strlen(base_url) + strlen(link) + 1);
                        strcpy(new_link, base_url);
                        strcat(new_link, link);
                        free(link);
                        link = new_link;
                    }
                    // Handle "www." links by prepending "http://"
                    else if (strncmp(link, "www.", 4) == 0) {
                        char *new_link = (char *)malloc(strlen("http://") + strlen(link) + 1);
                        strcpy(new_link, "http://");
                        strcat(new_link, link);
                        free(link);
                        link = new_link;
                    }

                    // Insert the URL into the hashmap and queue if it's not already visited
                    if (!search_url(hashmap, link)) {
                        insert_url(hashmap, link);  // Add to hashmap
                        enqueue(q, link, depth + 1);  // Add to the queue
                        printf("Found link: %s\n", link);
                    }
                }

                free(link);  // Free allocated memory for the link
            }
        }

        pos++;  // Move to the next position to search for more <a> tags
    }
}

// Function to fetch HTML content from a URL
void gethtml(CURL *curl, char *url, ResponseData *response) {
    curl_easy_setopt(curl, CURLOPT_URL, url);  // Set the target URL
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);  // Set the write callback function
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);  // Follow redirects if necessary
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)response);  // Pass response data struct

    CURLcode res = curl_easy_perform(curl);  // Perform the HTTP request
    if (res != CURLE_OK) {
        fprintf(stderr, "Invalid URL: %s\n", curl_easy_strerror(res));  // Log error if request fails
    }
}

// Stop words list
const char *stop_words[] = {
    "the", "and", "in", "of", "to", "a", "with", "for", "is", "on", "by", "this", "it", "at", "from", "or"
};
const int stop_words_count = sizeof(stop_words) / sizeof(stop_words[0]);

// Check if a word is a stop word
int is_stop_word(const char *word) {
    for (int i = 0; i < stop_words_count; i++) {
        if (strcasecmp(word, stop_words[i]) == 0) {
            return 1;  // Return true if it is a stop word
        }
    }
    return 0;  // Not a stop word
}

// Function to extract the title and meta tag contents from HTML
void get_keywords(trie *t, char *html, char *url) {
    HashMap h;
    init_hashmap(&h); // Initialize a hashmap to track processed keywords
    
    // Pointer to find the <title> tag in the HTML
    const char *title_tag_start = "<title>";
    const char *title_tag_end = "</title>";
    
    char *title_start = strstr(html, title_tag_start); // Locate <title> tag start
    int keyword_count = 0; // Count of unique keywords extracted
    char **keywords = malloc(100 * sizeof(char *)); // Allocate memory for keywords
    char *text_copy;
    char *token;

    // Extract the title content if the <title> tag is found
    if (title_start) {
        title_start += strlen(title_tag_start); // Move to the content of the title
        char *title_end = strstr(title_start, title_tag_end);
        
        if (title_end) {
            size_t title_length = title_end - title_start; // Calculate title length
            char *title = (char *)malloc(title_length + 1); // Allocate memory for the title
            strncpy(title, title_start, title_length); // Copy title content
            title[title_length] = '\0'; // Null-terminate the title
            
            text_copy = strdup(title); // Duplicate the title text for tokenization
            token = strtok(text_copy, " ,.-&:;/#%\\"); // Tokenize using common delimiters
            
            while (token != NULL) {
                // Convert token to lowercase for case-insensitive comparisons
                for (char *p = token; *p; p++) *p = tolower(*p);

                // Add token to keywords if it's not a stop word and not already processed
                if (!is_stop_word(token) && !search_url(&h, token)) {
                    insert_url(&h, token); // Mark the token as processed
                    keywords[keyword_count] = strdup(token); // Save the keyword
                    insert_trie(t, token, url); // Insert into the trie
                    (keyword_count)++; // Increment keyword count
                }
                token = strtok(NULL, " ,.-&:;/#%\\"); // Get the next token
            }

            free(text_copy); // Free the duplicated text
            free(title); // Free the allocated memory for the title
        }
    }

    // Process <meta> tags to extract keywords or descriptions
    const char *meta_tag_start = "<meta ";
    const char *meta_name_attr = "name=\"";
    const char *meta_content_attr = "content=\"";
    
    const char *meta_pos = html; // Pointer to traverse the HTML
    while ((meta_pos = strstr(meta_pos, meta_tag_start)) != NULL) {
        const char *name_pos = strstr(meta_pos, meta_name_attr); // Find 'name' attribute
        const char *content_pos = strstr(meta_pos, meta_content_attr); // Find 'content' attribute
        
        if (name_pos && content_pos) {
            // Extract the 'name' attribute value
            name_pos += strlen(meta_name_attr);
            const char *name_end = strchr(name_pos, '"');
            size_t name_length = name_end - name_pos;
            char *name = (char *)malloc(name_length + 1);
            strncpy(name, name_pos, name_length);
            name[name_length] = '\0';
            
            // Check if the name attribute is "keywords" or "description"
            if (strcmp(name, "keywords") == 0 || strcmp(name, "description") == 0) {
                // Extract the 'content' attribute value
                content_pos += strlen(meta_content_attr);
                const char *content_end = strchr(content_pos, '"');
                size_t content_length = content_end - content_pos;
                char *content = (char *)malloc(content_length + 1);
                strncpy(content, content_pos, content_length);
                content[content_length] = '\0';

                text_copy = strdup(content); // Duplicate the content for tokenization
                token = strtok(text_copy, " ,.-&:;/#%\\"); // Tokenize the content
                
                while (token != NULL) {
                    // Convert token to lowercase
                    for (char *p = token; *p; p++) *p = tolower(*p);

                    // Add token to keywords if it's valid
                    if (!is_stop_word(token) && !search_url(&h, token)) {
                        insert_url(&h, token); // Mark as processed
                        keywords[keyword_count] = strdup(token); // Save the keyword
                        insert_trie(t, token, url); // Insert into the trie
                        (keyword_count)++; // Increment keyword count
                    }
                    token = strtok(NULL, " ,.-&:;/#%\\"); // Get the next token
                }

                free(text_copy); // Free duplicated content
                free(content); // Free allocated content memory
            }
            free(name); // Free allocated name memory
        }
        meta_pos++; // Move to the next potential meta tag
    }
}

// Main function to initiate the web crawler
int main(int argc, char **argv) {
    HashMap hashmap;
    init_hashmap(&hashmap); // Initialize hashmap
    
    trie t;
    init_trie(&t); // Initialize trie

    CURL *curl;
    ResponseData response;
    response.data = (char *)malloc(1); // Initialize response buffer
    response.size = 0;
    curl = curl_easy_init(); // Initialize CURL library
    
    int maxdepth = atoi(argv[1]); // Set maximum crawling depth from input

    // Process each seed URL
    for (int i = 2; i < argc; i++) {
        char *seed_url = argv[i];
        int depth = 0;
        printf("\nThe current seed URL is: %s\n", seed_url);
        
        if (curl) {
            gethtml(curl, seed_url, &response); // Fetch HTML content
            
            queue q;
            qinit(&q); // Initialize the queue
            printf("\nCurrent depth level: %d\n\n", depth);
            find_links(seed_url, response.data, &hashmap, &q, 0); // Extract links
            get_keywords(&t, response.data, seed_url); // Extract keywords
            
            free(response.data);
            response.data = (char *)malloc(1);
            response.size = 0;

            node *q_node;
            while ((q_node = dequeue(&q)) && q_node->depth <= maxdepth) {
                if (depth < q_node->depth) {
                    printf("\nVisiting depth level: %d\n", q_node->depth);
                }
                printf("\nVisiting link: %s\n", q_node->url);
                gethtml(curl, q_node->url, &response); // Fetch HTML
                find_links(q_node->url, response.data, &hashmap, &q, q_node->depth); // Extract links
                get_keywords(&t, response.data, q_node->url); // Extract keywords

                free(response.data);
                response.data = (char *)malloc(1);
                response.size = 0;
                depth = q_node->depth;
                free(q_node);
            }
        } else {
            fprintf(stderr, "Failed to initialize CURL.\n");
        }
    }

    printf("\nCrawling complete!\n\n");
    curl_easy_cleanup(curl); // Cleanup CURL

    int choice = 1;
    char keyword[100];

    while (choice) {
        printf("Enter 1 to search for a keyword\n");
        printf("Enter 0 to exit\n");
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                printf("Enter the keyword: ");
                scanf("%s", keyword);
                search_trie(&t, keyword); // Search for the keyword in the trie
                break;
            case 0:
                break;
            default:
                break;
        }
        printf("\n");
    }

    return 0;
}

