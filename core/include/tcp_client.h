#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

typedef struct {
    int socket_fd;
    char host[128];
    unsigned short port;
    int is_connected;
} tcp_client;

int tcp_client_init(tcp_client *client, unsigned short port, const char *host);
int tcp_client_connect(tcp_client *client);
int tcp_client_write(tcp_client *client, const unsigned char *data, int length);
int tcp_client_read(tcp_client *client, unsigned char *buffer, int length);
void tcp_client_dispose(tcp_client *client);

#endif
