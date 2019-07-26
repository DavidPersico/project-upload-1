/*
 *  Contains response handler for client connections.
 *
 *  by David Persico
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "concat.h"
#include "globals.h"
#include "file_info.h"
#include "response_handler.h"

/* Recieve client request and send server response */
void *response_handler(void* ptr)
{
    char request[99999];
    char *request_line[2];  /* For request parsing */
    char buffer[BUFSIZE];   /* Holds data we send */
    char send_file_path[99999];
    char *packet_header[4];

    /* Unpack and free args */
    client_args* args = (client_args*)(ptr);
    int slot = args->client;
    char *server_root = globals.server_root;
    free(args);

    /* Clear memory and recieve client request */
    memset((void*)request, (int)'\0', 99999);
    int rcvd = recv(globals.clientConnection[slot], request, 99999, 0);
    if (rcvd < 0)
        /* Use perror in threads as exit() in error() kills all threads */
        perror("recv failed");
    else if (rcvd == 0)
        perror("Client upexpectedly disconnected");
    /* Request received, begin parsing */
    else
    {
        printf("Request:\n%s", request);
        request_line[0] = strtok(request, " \t\n");
        if (strncmp(request_line[0], "GET\0", 4) == 0)
        {
            request_line[1] = strtok(NULL, " \t");
            request_line[2] = strtok(NULL, " \t\n");
            /* Check Request */
            if (strncmp(request_line[2], "HTTP/1.0", 8) != 0 && strncmp(request_line[2], "HTTP/1.1", 8) != 0)
                write(globals.clientConnection[slot], "HTTP/1.1 500 Internal  Error\n", 35);
            else
            {
                if (strncmp(request_line[1], "/\0", 2) == 0)
                    request_line[1] = "/index.html"; /* Default to index.html */
                snprintf(send_file_path, sizeof(send_file_path), "%s", server_root);
                strcpy(&send_file_path[strlen(server_root)], request_line[1]);
                printf("File to send: %s\n\n", send_file_path);
                int fd = open(send_file_path, O_RDONLY);
                if (fd == -1) /* File not found */
                    write(globals.clientConnection[slot], "HTTP/1.1 404 File Not Found\n", 28);
                else          /* File found at correct path */
                {
                    /* Construct, send and free packet_header */
                    packet_header[0] = get_file_size(send_file_path);
                    packet_header[1] = concat(packet_header[0], "\r\n\r\n");
                    packet_header[2] = concat("\r\n Content-Length: ", packet_header[1]);
                    packet_header[3] = concat(get_file_ext(send_file_path), packet_header[2]);
                    packet_header[4] = concat("HTTP/1.1 200 Document Follows\r\n Content-Type: ", packet_header[3]);
                    send(globals.clientConnection[slot], packet_header[4], strlen(packet_header[4]), 0);
                    printf("Response packet header:\n%s\n\n", packet_header[4]);
                    for (int i = 0; i < 5; i++)
                        free(packet_header[i]);

                    /* Read file and write to client */
                    int read_data = read(fd, buffer, BUFSIZE);
                    while (read_data > 0)
                    {
                        write(globals.clientConnection[slot], buffer, read_data);
                        read_data = read(fd, buffer, BUFSIZE);
                    }
                    close(fd);
                }
            }
        }
    }
    /* Shutdown, close socket and mark connection slot open */
    shutdown(globals.clientConnection[slot], SHUT_RDWR);
    close(globals.clientConnection[slot]);
    globals.clientConnection[slot]=-1;
    pthread_exit(0);
}
