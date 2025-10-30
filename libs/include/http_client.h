#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include "../../core/include/tcp_client.h"

/* function pointer (callback) */
typedef void (*http_response_handler)(const char *data, unsigned long len);

/* send POST */
int http_client_post(tcp_client *client,
                     const char *host,
                     const char *path,
                     const char *json_body,
                     http_response_handler handler);

/* read all response bytes into newly allocated *out */
int http_client_read_all(tcp_client *client,
                         char **out,
                         unsigned long *out_len,
                         unsigned long max_bytes);

#endif
