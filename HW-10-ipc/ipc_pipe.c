#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <stdlib.h>
#include <sys/wait.h>

#define PING "ping"


int main(void)
{
	printf("create process with id =%d\n\n", getpid());

	pid_t pid;
#if 0
	if( (pid = fork()) != 0) // parent process
	{
		printf("parent process have id =%d\n", getpid());
	}
	else
	{
		printf("child process have id =%d\n", getpid());
	}
#endif
	
	for(int i = 0; i < 3; i++)
	{
		int fd[2];
		pipe(fd);
		printf("pipe_in = %d, pipe_out = %d\n", fd[0], fd[1]);
		if ( (pid = fork()) == 0)
		{
			printf("{%d} [%d] -> [%d]\n", i, getppid(), getpid());

			char to_out[80];
			sprintf(to_out, "%s from %d \n", PING, getpid());
			
			close(fd[0]);

			write(fd[1], to_out, strlen(to_out));
			exit(0);
		}
#if 1
		else
		{
			int statloc = 0;
			waitpid(pid, &statloc, 0);

			close(fd[1]);

			char from_child[30];
			read(fd[0], &from_child, 30);

			printf("{%d} [%d] <- [%d] :%s\n",i, getpid(), pid, from_child);
		}
#endif
	}
#if 0
	if (pid != 0)	
	{
		int statloc = 0;
		waitpid(pid, &statloc, 0);
		sleep(2);
		close(fd[1]);
		char from_child[30];
		read(fd[0], &from_child, 30);
		printf("from child :\n%s\n", from_child);
	}
#endif
	return 0;
}

static void fork_child(void)
{
	fork();
	printf("%d -> %d\n", getppid(), getpid());
}
