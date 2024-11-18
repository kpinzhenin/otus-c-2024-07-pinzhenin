#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
// for FIFO using
#include <sys/stat.h>
#include <sys/wait.h>

#include <fcntl.h>

#define FIFO_NAME "my_FIFO2"
#define PING "ping"

int main(void)
{
	printf("create process with id =%d\n\n", getpid());

	// create fifo
	umask(0); // reset permission for next file
	if ( mkfifo(FIFO_NAME, 0666) == -1) // last type dev_t for device using
	{
		perror("error create fifo");
	//	exit(1);
	}

	pid_t pid = fork();
	if (pid == 0) //child process
	{
		int fd = open(FIFO_NAME, O_WRONLY);
		char msg[30] = "to parent";
		write(fd, msg, strlen(msg) + 1);
		close(fd);
		exit(0);
	}
	else
	{
		int fd = open(FIFO_NAME, O_RDONLY);
		char in_msg[30];
		read(fd, in_msg, 30);
		close(fd);

		printf("from child: %s\n", in_msg);
	}
//	printf("return %d\n", fd);
//	close(fd);
	unlink(FIFO_NAME);
	return 0;
}
