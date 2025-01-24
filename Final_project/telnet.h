#ifndef _H_TELNET
#define _H_TELNET

#include <winsock2.h>

#define RTT_TELNET_PORT 19021 // RTT port

int Telnet_socket_init(SOCKET *sock);
int Telnet_socket_connect(SOCKET *sock);
int Telnet_socket_send_msg(SOCKET *sock, const char* msg);
int Telnet_socket_receive(SOCKET *sock, char *recv_buffer, size_t buffer_size);

#endif