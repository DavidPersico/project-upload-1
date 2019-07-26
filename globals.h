#ifndef GLOBALS_H
#define GLOBALS_H

/* Client connection limit and packet size */
#define MAXCLIENTS 1000
#define BUFSIZE 1024

/* Wrapper for passing args into pthread_create()*/
typedef struct
{
    int client;
} client_args;

/* Global variable store */
typedef struct
{
    int clientConnection[MAXCLIENTS];
    char *server_root;
} Globals;

extern Globals globals;

#endif /* GLOBALS_H */
