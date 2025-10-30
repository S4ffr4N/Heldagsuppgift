#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../core/include/tcp_client.h"
#include "../../libs/include/http_client.h"

/* find start of HTTP body after \r\n\r\n */
static char* find_http_body(char *response)
{
    int i = 0;
    if (!response) return 0;

    while (response[i] != '\0') {
        if (response[i] == '\r' &&
            response[i+1] == '\n' &&
            response[i+2] == '\r' &&
            response[i+3] == '\n') {
            return &response[i+4];
        }
        i++;
    }
    return 0;
}

int main(void)
{
    /* Declarations */
    
    /* Fixed test server for demo purpose */
    const char *host = "httpbin.org";
    unsigned short port = 80;
    const char *path = "/post";

    /* Hardcoded values for demo purpose */
    char json_body[256];
    const char *device = "NODE001";
    const char *time_str = "2025-10-30T21:00:00Z";
    const char *temperature = "21.5C";

    tcp_client client;
    char *resp = 0;
    unsigned long resp_len = 0;
    char *body;


    /* build JSON body */
    json_body[0] = '\0';
    strcpy(json_body, "{ \"device\":\"");
    strcat(json_body, device);
    strcat(json_body, "\", \"time\":\"");
    strcat(json_body, time_str);
    strcat(json_body, "\", \"temperature\":\"");
    strcat(json_body, temperature);
    strcat(json_body, "\" }");

    if (tcp_client_init(&client, port, host) != 0) return 1;
    if (tcp_client_connect(&client) != 0) return 2;

    /* pass NULL instead of callback to avoid printing anything before body */
    if (http_client_post(&client, host, path, json_body, 0) != 0) {
        printf("http_client_post error\n");
    }

    if (http_client_read_all(&client, &resp, &resp_len, 0UL) == 0 && resp) {
        body = find_http_body(resp);
        if (body) {
            printf("%s", body);
        } else {
            /* fallback: print whole response if no header/body split found */
            printf("%s", resp);
        }
        free(resp);
    }

    tcp_client_dispose(&client);
    return 0;
}
