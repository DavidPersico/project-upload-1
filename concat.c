/*
 *  Contains method to combine strings.
 *
 *  by David Persico
 */

#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "concat.h"

/* Combine two strings into one */
char *concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    if (result == NULL)
        error("concat function malloc()");
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}
