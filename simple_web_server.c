/*
 *  This is a very simple HTTP web server written in C. It uses the "www" folder
 *  in the current directory as server root and only handles GET requests.
 *
 *  Usage is ./web_server <PORT> <IP>
 *
 *  Written by David Persico
 */
#include<stdio.h>
#include<netdb.h>
#include<unistd.h>
#include<string.h>
#include<signal.h>
#include<stdlib.h>
#include<pthread.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#include "error.h"
#include "concat.h"
#include "globals.h"
#include "file_info.h"
#include "response_handler.h"
#include "interrupt_handler.h"

int main(int argc , char **argv)
{
    /* Connection infomation */
    struct addrinfo hints, *res, *p;
    struct sockaddr_in clientaddr;
    socklen_t addrlen = sizeof(clientaddr);

    int listenfd; /* Listen file descriptor, used for incoming connections */
    int err;
    int slot = 0; /* The client we are servicing */

    /* Check if port number in args */
    if (argc != 3)
        error("Usage: ./web_server <PORT> <IP>");

    /* Mark each clientConnection spot as -1 for unused */
    for (int i=0; i<MAXCLIENTS; i++)
        globals.clientConnection[i] = -1;

    /* Allows broken pipe/connection without killing progam */
    signal(SIGPIPE, SIG_IGN);

    /* Call interrupt handler on CTRL-C or CTRL-Z */
    signal(SIGINT, interrupt_handler);
    signal(SIGTSTP, interrupt_handler);

    /* Set  address specifications and get address */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;       /* AF_INET specifies IPv4 protocol */
    hints.ai_socktype = SOCK_STREAM; /* SOCK_STREAM specifies TCP */
    hints.ai_flags = AI_PASSIVE;     /* AI_PASSIVE specifies wildcard IP addresss */
    if (getaddrinfo(argv[1], argv[2], &hints, &res) != 0)
        error("getaddrinfo() error");

    /* Set listenfd to socket and bind */
    for (p = res; p != NULL; p = p -> ai_next)
    {
        listenfd = socket(p -> ai_family, p -> ai_socktype, 0);
        if (listenfd == -1)
            continue;
        if (bind(listenfd, p -> ai_addr, p -> ai_addrlen) == 0)
            break;
    }
    if (p == NULL)
        error("socket() and bind()");

    freeaddrinfo(res);

    /* Set directory of website we are hosting */
    globals.server_root = concat(getenv("PWD"),"/www");

    /* Begin listening for incoming connections */
    if (listen(listenfd, 999990) != 0)
        error("listen() error");

    /* Handler for accepting incoming connections */
    while (1)
    {
        globals.clientConnection[slot] = accept(listenfd, (struct sockaddr *) &clientaddr, &addrlen);
        if (globals.clientConnection[slot] < 0)
            error("accept() error");
        else
        {
            /* Handle new client connections with new threads via pthread_create() */
            pthread_t thread;
            client_args* args = malloc(sizeof(client_args));
            if (args == NULL)
                error("Main() client_args malloc()");
            args->client = slot;
            err = pthread_create(&thread, NULL, response_handler, (void*)(args));
            if (err != 0)
            {
                fprintf(stderr, "pthread_create: %s", strerror(err));
                close(args->client);
                free(args);
                continue;
            }
            err = pthread_detach(thread);
            if (err != 0)
            {
                fprintf(stderr, "pthread_detach: %s", strerror(err));
                continue;
            }
        }
        while (globals.clientConnection[slot] !=-1)
            slot = (slot + 1) % MAXCLIENTS;
    }
    return 0;
}
