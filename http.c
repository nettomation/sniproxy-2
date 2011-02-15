#include <stdio.h>
#include <string.h> /* strncpy() */
#include "http.h"
#include "util.h"

#define SERVER_NAME_LEN 256


static int next_header(const char **, int *);


const char *
parse_http_header(const char* data, int data_len) {
    static char server_name[SERVER_NAME_LEN];
    int len, i;

    /* loop through headers stopping at first blank line */
    while ((len = next_header(&data, &data_len)) != 0)
        if (len > 6 && strncmp("Host: ", data, 6) == 0) {
            strncpy (server_name, data + 6, len - 6);
            server_name[len - 6] = '\0';

            /* trim off port portion */
            for (i = len - 1; i > 0; i--)
                if (server_name[i] == ':') {
                    server_name[i] = '\0';
                    break;
                }

            return server_name;
        }

    return NULL;
}

static int
next_header(const char **data, int *len) {
    int header_len;

    /* perhaps we can optimize this to reuse the value of header_len, rather than scanning twice */
    /* walk our data stream until the end of the header */
    while (*len > 2 && (*data)[0] != '\r' && (*data)[1] != '\n') {
        (*len) --;
        (*data) ++;
    }

    /* advanced past the <CR><LF> pair */
    *data += 2;
    *len -= 2;

    /* Find the length of the next header */
    header_len = 0;
    while (*len > header_len + 1
            && (*data)[header_len] != '\r'
            && (*data)[header_len + 1] != '\n')
        header_len++;

    return header_len;
}
