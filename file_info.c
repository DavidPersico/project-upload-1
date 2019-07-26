/*
 *  Contains file size and extension getters.
 *
 *  by David Persico
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "error.h"
#include "file_info.h"

/* Get size of the file */
char *get_file_size(const char *file_name)
{
    struct stat st;
    int length = 1;
    if (stat(file_name, &st) == 0)
        /* Call with NULL gives string size */
        length = snprintf(NULL, 0, "%ld", st.st_size) + 1;
    else
        error("get_file_size()");
    char *result = malloc(length);
    if (result == NULL)
        error("get_file_size(): function malloc()");
    snprintf(result, length, "%ld", st.st_size);
    return result;
}

/* Get file extension */
const char *get_file_ext(const char *file_name)
{
    const char *dot = strrchr(file_name, '.');
    if(!dot || dot == file_name)
        return "";
    return dot + 1;
}
