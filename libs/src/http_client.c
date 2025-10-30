#include "../include/http_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* send all bytes */
static int http_client_send_all(tcp_client *client, const unsigned char *data, int length)
{
    int sent = 0;
    int n;

    if (!client || !data || length <= 0)
        return -1;

    while (sent < length)
    {
        n = tcp_client_write(client, data + sent, length - sent);
        if (n <= 0)
            return -1;
        sent += n;
    }
    return sent;
}

/* convert unsigned long to ascii */
static int write_uint_to_buffer(char *buf, unsigned long value)
{
    char temp[32];
    int i = 0;
    int j;

    if (!buf)
        return -1;

    if (value == 0)
    {
        buf[0] = '0';
        buf[1] = '\0';
        return 1;
    }

    while (value > 0 && i < (int)(sizeof(temp) - 1))
    {
        temp[i++] = (char)('0' + (value % 10));
        value /= 10;
    }

    /* reverse */
    for (j = 0; j < i; ++j)
        buf[j] = temp[i - j - 1];

    buf[i] = '\0';
    return i;
}

int http_client_post(
    tcp_client *client,
    const char *host,
    const char *path,
    const char *json_body,
    http_response_handler handler)
{
    char request[2048];
    char content_len_buf[32];
    int pos = 0;
    unsigned long body_len;

    if (!client || !host || !path)
        return -1;

    if (!json_body)
        json_body = "";

    body_len = (unsigned long)strlen(json_body);

    /* convert length */
    if (write_uint_to_buffer(content_len_buf, body_len) < 0)
        return -1;

    /* build request */
    {
        const char *s;

        /* Request line */
        s = "POST ";                  strcpy(request + pos, s); pos += (int)strlen(s);
        s = path;                     strcpy(request + pos, s); pos += (int)strlen(s);
        s = " HTTP/1.1\r\n";          strcpy(request + pos, s); pos += (int)strlen(s);

        /* Host header */
        s = "Host: ";                 strcpy(request + pos, s); pos += (int)strlen(s);
        s = host;                     strcpy(request + pos, s); pos += (int)strlen(s);
        s = "\r\n";                   strcpy(request + pos, s); pos += (int)strlen(s);

        /* Content-Type */
        s = "Content-Type: application/json\r\n";
                                     strcpy(request + pos, s); pos += (int)strlen(s);

        /* Content-Length */
        s = "Content-Length: ";       strcpy(request + pos, s); pos += (int)strlen(s);
        s = content_len_buf;          strcpy(request + pos, s); pos += (int)strlen(s);
        s = "\r\n";                   strcpy(request + pos, s); pos += (int)strlen(s);

        /* Connection: close + blank line */
        s = "Connection: close\r\n";  strcpy(request + pos, s); pos += (int)strlen(s);
        s = "\r\n";                   strcpy(request + pos, s); pos += (int)strlen(s);
    }
    /* =========================================================== */

    /* send headers */
    if (http_client_send_all(client, (unsigned char*)request, pos) < 0)
        return -1;

    /* send body */
    if (body_len > 0)
    {
        if (http_client_send_all(client, (unsigned char*)json_body, (int)body_len) < 0)
            return -1;
    }

    /* notify via callback */
    if (handler) {
        const char *msg = "[client] POST sent";
        handler(msg, (unsigned long)strlen(msg));
    }

    return 0;
}

int http_client_read_all(
    tcp_client *client,
    char **out,
    unsigned long *out_len,
    unsigned long max_bytes)
{
    unsigned char buffer[1024];
    char *result = NULL;
    unsigned long used = 0;
    unsigned long cap = 0;
    int n;

    if (!client || !out)
        return -1;

    *out = NULL;
    if (out_len)
        *out_len = 0;

    for (;;)
    {
        n = tcp_client_read(client, buffer, (int)sizeof(buffer));
        if (n < 0)
        {
            free(result);
            return -1;
        }
        if (n == 0)
            break;

        if (max_bytes > 0 && used + (unsigned long)n > max_bytes)
            n = (int)(max_bytes - used);

        if (used + (unsigned long)n + 1 > cap)
        {
            unsigned long newcap = (cap == 0) ? 4096 : cap * 2;
            while (newcap < used + (unsigned long)n + 1)
                newcap *= 2;

            result = (char*)realloc(result, newcap);
            if (!result)
                return -1;
            cap = newcap;
        }

        memcpy(result + used, buffer, (size_t)n);
        used += (unsigned long)n;

        if (max_bytes > 0 && used >= max_bytes)
            break;
    }

    if (!result)
    {
        result = (char*)malloc(1);
        if (!result)
            return -1;
        result[0] = '\0';
    }
    else
        result[used] = '\0';

    *out = result;
    if (out_len)
        *out_len = used;

    return 0;
}
