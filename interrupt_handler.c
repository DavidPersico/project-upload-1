/*
 *  Contains interrupt handler. Responsible for closing client connections.
 *  Closing client connections allows ports to be reused.
 *
 *  by David Persico
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

#include "response_handler.h"
#include "interrupt_handler.h"

/* Close all open connections */
void interrupt_handler(int sig)
{
    printf("Interrupt!\nClosing open connections..\nReceived signal %d\n", sig);
    for (int i=0; i<MAXCLIENTS; i++)
    {
        if (clientConnection[i] > -1)
        {
            shutdown(clientConnection[i],  SHUT_RDWR);
            close(clientConnection[i]);
        }
    }
    exit(0);
}
