#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#include "telnet.h"

// #pragma comment(lib, "ws2_32.lib")

WSADATA wsaData;
//#define RTT_TELNET_PORT 19021 // RTT port

int Telnet_socket_init(SOCKET *sock)
{
	// WinSock initialisation
	//WSADATA wsaData;	
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0 )
	{
		// for print made pointer to signal print msg ?????? or move print signal in separate header and include it hear
		printf("Error to create socket. Error Code: %d\n", WSAGetLastError());
        WSACleanup();
        return -1;
	}
	
	// Create socket 
	*sock = socket(AF_INET, SOCK_STREAM, 0);
    if (*sock == INVALID_SOCKET) {
        printf("Error to create socket. Error Code: %d\n", WSAGetLastError());
        WSACleanup();
        return -1;
    }
	
	return 0; // success status 
}
	
int Telnet_socket_connect(SOCKET *sock)
{
	struct sockaddr_in server =
	{
	.sin_family = AF_INET,
	.sin_port = htons(RTT_TELNET_PORT),
	.sin_addr.s_addr = inet_addr("127.0.0.1")
	};

	if (connect(*sock, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
        printf("Connection error. Error Code: %d\n", WSAGetLastError());
        closesocket(*sock);
        WSACleanup();
        return -1;
    }
	
	return 0;
}
// just wrapper of send function
int Telnet_socket_send_msg(SOCKET *sock, const char* msg)
{
		
	send(*sock, msg, strlen(msg), 0);
	
	return 0;
}

// bloking function of wait socket answer
int Telnet_socket_receive(SOCKET *sock, char *recv_buffer, size_t buffer_size)
{
	int recv_size = recv(*sock, recv_buffer, buffer_size - 1, 0);
	if (recv_size == SOCKET_ERROR) {
		printf("Error receive data from server. Error Code: %d\n", WSAGetLastError());
		return -1;
	}
	
	recv_buffer[recv_size] = 0x0; // insert null terminator in end
	
	return recv_size;	
}