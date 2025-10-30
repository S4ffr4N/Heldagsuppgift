#include "../include/tcp_client.h"

#include <string.h>
#include <unistd.h>         /* close() */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>          /* gethostbyname() */

/*
 * Resolve hostname and establish TCP connection (IPv4)
 * Returns socket file descriptor or -1 on error
 */
static int tcp_client_resolve_and_connect(const char *host, unsigned short port)
{
    struct hostent *host_entry;
    struct sockaddr_in server_addr;
    int socket_fd;

    if (!host)
        return -1;

    /* DNS lookup */
    host_entry = gethostbyname(host);
    if (!host_entry)
        return -1;

    /* Create TCP socket */
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
        return -1;

    /* IPv4 address structure */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(port);

    /* Copy resolved IP into sockaddr struct */
    memcpy(&server_addr.sin_addr, host_entry->h_addr_list[0], host_entry->h_length);

    /* Establish TCP connection */
    if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        close(socket_fd);
        return -1;
    }

    return socket_fd;
}

int tcp_client_init(tcp_client *client, unsigned short port, const char *host)
{
    unsigned int i = 0;

    if (!client || !host)
        return -1;

    client->socket_fd = -1;
    client->port = port;
    client->is_connected = 0;

    /* Copy hostname to internal buffer */
    while (host[i] != '\0' && i < sizeof(client->host) - 1)
    {
        client->host[i] = host[i];
        i++;
    }
    client->host[i] = '\0';

    return 0;
}

int tcp_client_connect(tcp_client *client)
{
    int socket_fd;

    if (!client)
        return -1;

    socket_fd = tcp_client_resolve_and_connect(client->host, client->port);
    if (socket_fd < 0)
        return -1;

    client->socket_fd = socket_fd;
    client->is_connected = 1;

    return 0;
}

/* Send raw bytes over TCP */
int tcp_client_write(tcp_client *client, const unsigned char *data, int length)
{
    ssize_t bytes_sent;

    if (!client || client->socket_fd < 0 || !data || length <= 0)
        return -1;

    bytes_sent = send(client->socket_fd, (const void*)data, (size_t)length, 0);
    if (bytes_sent < 0)
        return -1;

    return (int)bytes_sent;
}

/* Receive bytes from TCP socket */
int tcp_client_read(tcp_client *client, unsigned char *buffer, int length)
{
    ssize_t bytes_read;

    if (!client || client->socket_fd < 0 || !buffer || length <= 0)
        return -1;

    bytes_read = recv(client->socket_fd, (void*)buffer, (size_t)length, 0);
    if (bytes_read < 0)
        return -1;

    return (int)bytes_read;
}

/* Close connection */
void tcp_client_dispose(tcp_client *client)
{
    if (!client)
        return;

    if (client->socket_fd >= 0)
    {
        close(client->socket_fd);
        client->socket_fd = -1;
    }
    client->is_connected = 0;
}
