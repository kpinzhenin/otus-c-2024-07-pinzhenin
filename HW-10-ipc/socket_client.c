
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_NAME "some_socke"



int main(void)
{
	// create socket
	int server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (server_sock == -1)
	{
		perror("socket create");
		exit(1);
	}
	struct sockaddr_un server_addr =
       	{
		.sun_family = AF_UNIX,
		.sun_path = SOCKET_NAME
	};

	// create server socket

	// connect to server
	if ( connect(server_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_un)) == -1)
	{
		perror("server connect");
		exit(1);
	}
#if 0
	// send message
	char msg[] = "hello server";
	write(server_sock, msg, strlen(msg) + 1);
#endif
	// read from server
	char msg[80];
	unsigned long int size = 0;
	size_t read_num = read(server_sock, &size, sizeof(long int));
	if (read_num > 0)
		printf("msg from server: %lu\n", size);

	close(server_sock);

	return 0;
}
