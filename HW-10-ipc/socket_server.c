#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// for socket
#include <sys/socket.h>
#include <sys/un.h>

// for file size
#include <sys/stat.h>

//for daemon create
#include <sys/resource.h>
#include <signal.h>
#include <fcntl.h>

static void read_config(char *socket_name, char *file_name);
static unsigned long int get_size_of_file(char *file_name);
static int create_server_client(char *sock_name);
void demonaize(void);

int main(int argc, char *argv[])
{
	// check input 
	if ( argc > 2 )
	{
		printf("too mane arg\n");
		exit(1);
	}
	else if ( argc == 2 ) // expect only options
	{
		// check for -d options
		if ( strcmp(argv[1],"-d") != 0 )
		{
			printf("options:%s\n", argv[1]);
			exit(1);
		}
		// create daemon
		printf("create daemon...\n");
		demonaize();
	}
	// read socket config
	char socket_name_buf[128], file_name[128];
	read_config(socket_name_buf, file_name);


#if 1
	// create socket
	char sock_name[strlen(socket_name_buf)];
	strcpy(sock_name, socket_name_buf);
	unlink(sock_name);
	int server_sock = create_server_client(sock_name);

	// make a listen connection
	if (listen(server_sock, 0) == -1) // "0" backlog for set min listen queue in system
	{
		perror("listen error");
		exit(1);
	}
	
	// accept link
	int client_sock = accept(server_sock, NULL, NULL);
	if (client_sock == -1)
	{
		perror("accept");
		exit(1);
	}
	unsigned long int file_size = 
		get_size_of_file(file_name);

	// write to client
	char *msg = "hello from server";
	unsigned long int size = get_size_of_file(file_name);

	write(client_sock,&size, sizeof(long int));

	close(server_sock);
	close(client_sock);
	unlink(sock_name);
#endif
	return 0;
}

void demonaize(void)
{
	// reset mask of file creation
	// new create file by the process will have permission 777
	umask(0);

	// get max file descriptor number
	struct rlimit rl;
	if (getrlimit(RLIMIT_NOFILE, &rl) < 0 )
		perror("don't have max number of file descriptor\n");

	// create demon process
	pid_t pid;
	if ( (pid = fork()) < 0 )
		perror("error fork from process");
	else if ( pid != 0 ) // mean the parent process
		exit(0); // close the parent process

//======Next fork only child process (daemon)
	// make child process lead of session session will get number of pid
	setsid();
	
	// make ability don't have rule terminal in future
	struct sigaction sa;
	sa.sa_handler = SIG_IGN;
	sa.sa_flags = 0;
	//if (sigaction(SIGHUP, &sa, NULL) < 0)
	sigaction(SIGHUP, &sa, NULL); // reset SIGHUP handler ??

	if ( (pid = fork()) < 0 )
		perror("error fork from process");
	else if ( pid != 0 ) // mean the parent process
		exit(0); // close the parent process

	// close opened file descriptors
	if (rl.rlim_max == RLIM_INFINITY )
		rl.rlim_max = 1024;
	for (int i = 0; i < rl.rlim_max; i++)
		close(i);

	// join NULL descriptor to std stream from /dev/null
	int fd0 = open("/dev/null", O_RDWR);
	int fd1 = dup(0);
	int fd2 = dup(0);
}

static int create_server_client(char *sock_name)
{
	// create socket
	int server_sock =  socket(AF_UNIX, SOCK_STREAM, 0);
	// check create socket
	if (server_sock < 0)
	{
		perror("error to create socket");
		exit(1);
	}

	// set socket address
	struct sockaddr_un server_addr = 
	{
		.sun_family = AF_UNIX,
		//.sun_path = SOCKET_NAME
	};
	strcpy( server_addr.sun_path, sock_name);
	// bind address to socket
	int bind_res = bind(server_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_un));
	if(bind_res == -1)
	{
		perror("bind socket");
		exit(1);
	}
	return server_sock;
}

static void read_config(char *socket_name, char *file_name)
{
	/* read socket setting */
	// open config file
	FILE* config_fd = fopen(".config","r");
	if (config_fd == NULL)
	{
		perror("open config");
		exit(1);
	}

	// read socket name
	if(fscanf(config_fd, "socket_name=%s", socket_name) == 0)
	//	printf("read from config: %s\n", socket_name);
	//else
	{
		perror("wrong config file format: \"socket name\"\n");
		exit(1);
	}

	// read file name
	if ( fscanf(config_fd, " file_path=%s", file_name) == 0 )
	//	printf("file_path: %s\n", file_name);
	//else
	{
		perror("wrong config file format: \"file name\"\n");
		exit(1);
	}

	fclose(config_fd);
}

static unsigned long int get_size_of_file(char *file_name)
{
	struct stat statbuf;
	if ( stat(file_name, &statbuf) == -1)
	{
		perror("error in stat: ");
		exit(1);
	}
	return statbuf.st_size;
}
