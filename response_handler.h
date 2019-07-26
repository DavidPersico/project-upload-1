#ifndef RESPONSE_HANDLER_H
#define RESPONSE_HANDLER_H

typedef struct {
    int client;
    char *root;
} client_args;

/* Client connection limit and packet size */
#define MAXCLIENTS 1000
#define BUFSIZE 1024

int clientConnection[MAXCLIENTS];

void *response_handler(void* ptr);

#endif /* RESPONSE_HANDLER_H */
