# include "man.h"
// https://www.linuxhowtos.org/data/6/client.c

#define MAX_SIZE 4096
#define MAX_RESPONSE 65535

// Function to remove HTML tags
void strip_html_tags(char *str) {
    char *p = str;
    char *q = str;
    int in_tag = 0;
    
    while (*p) {
        if (*p == '<') {
            in_tag = 1;
        } else if (*p == '>') {
            in_tag = 0;
            p++;
            continue;
        }
        
        if (!in_tag) {
            *q++ = *p;
        }
        p++;
    }
    
    *q = '\0';
}

// Function to print content after `</head>`
void process_and_print_content(char *content) {
    char *start = strstr(content, "</head>");
    if (start) {
        start += strlen("</head>");
        // Process the content after </head>
        char body[MAX_RESPONSE];
        strncpy(body, start, sizeof(body) - 1);
        body[sizeof(body) - 1] = '\0';

        // Strip HTML tags
        strip_html_tags(body);

        // Print the cleaned content
        printf("\n%s\n", body);
    } else {
        printf("Could not find the end of the header\n");
    }
}

//Function to send request and get the header
void send_request(const char *topic) {
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char request[MAX_SIZE];
    char buffer[MAX_SIZE];
    const char *host = "man.he.net";
    const char *path_format = "/?topic=%s&section=all";
    char path[MAX_SIZE];
    char response[MAX_RESPONSE];
    int headers_ended = 0;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }
    
    // Resolve host
    server = gethostbyname(host);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(80);
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error connecting to server");
        exit(1);
    }

    // Create request path
    snprintf(path, sizeof(path), path_format, topic);
    snprintf(request, sizeof(request),
             "GET %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "User-Agent: CustomClient/1.0\r\n"
             "Connection: close\r\n\r\n",
             path, host
    );

    if (send(sockfd, request, strlen(request), 0) < 0) {
        perror("Error sending request");
        exit(1);
    }

    int bytes_received;
    char *response_ptr = response;

    // Read response
    while ((bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_received] = '\0'; // Null-terminate buffer

        // Accumulate response content
        if (response_ptr + bytes_received - response < sizeof(response)) {
            memcpy(response_ptr, buffer, bytes_received);
            response_ptr += bytes_received;
        } else {
            fprintf(stderr, "Response buffer overflow, Increase response buffer size\n");
            break;
        }
    }

    if (bytes_received < 0) {
        perror("Error receiving response");
    }

    // Null-terminate the response
    *response_ptr = '\0';

    // Process and print content after </head>
    process_and_print_content(response);

    close(sockfd);
}


int iMan_handler(char **strings, int index) {
    if (strings[index] == NULL) {
        printf("No arguments\n");
        return 1;
    }
    send_request(strings[index]);
    return 0;
}
