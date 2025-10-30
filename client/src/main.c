#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../core/include/tcp_client.h"
#include "../../libs/include/http_client.h"

/* function pointer demo: prints server response */
static void print_response_callback(const char *data, unsigned long len)
{
    (void)len;
    if (data) {
        printf("%s\n", data);
    }
}

int main(void)
{
    
    /* Declarations */

    /* Hardcoded prototype config 
    const char *host = "httpbin.org";
    unsigned short port = 80;
    const char *path = "/post"; */

    /* Placeholder body for prototype (we can swap to device/time/temperature later) */
    const char *host = "httpbin.org";
    unsigned short port = 80;
    const char *path = "/post";
    
    /* JSON body */
    char json_body[256];
    const char *device = "NODE001";
    const char *time_str = "2025-10-30T21:00:00Z";
    const char *temperature = "21.5C";
    
    tcp_client client;
    char *resp = 0;
    unsigned long resp_len = 0;


    /* reset buffer and append fields */
    json_body[0] = '\0';
    strcpy(json_body, "{ \"device\":\"");
    strcat(json_body, device);
    strcat(json_body, "\", \"time\":\"");
    strcat(json_body, time_str);
    strcat(json_body, "\", \"temperature\":\"");
    strcat(json_body, temperature);
    strcat(json_body, "\" }");



    if (tcp_client_init(&client, port, host) != 0)
        return 1;

    if (tcp_client_connect(&client) != 0)
        return 2;

    if (http_client_post(&client, host, path, json_body, print_response_callback) != 0) {
        printf("http_client_post error\n");
    }

    if (http_client_read_all(&client, &resp, &resp_len, 0UL) == 0 && resp) {
         fwrite(resp, 1, (size_t)resp_len, stdout);
        /* resp contains the full HTTP response as a string */
        /* printf("[read_all] %lu bytes\n", resp_len); */
        free(resp);
    }

    tcp_client_dispose(&client);
    return 0;
}
