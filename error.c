/*
 *  Contains wrapper for perror.
 *
 *  by David Persico
 */

#include <stdlib.h>
#include <stdio.h>

#include "error.h"

/* Error, a wrapper for perror */
void error(char *msg)
{
    perror(msg);
    exit(1);
}
