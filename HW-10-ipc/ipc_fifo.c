#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
// for FIFO using
#include <sys/stat.h>
#include <sys/wait.h>

#include <fcntl.h>

#define FIFO_NAME "my_fifo"
#define PING "ping"

int main(void)
{

	printf("create process with id =%d\n\n", getpid());

	pid_t pid;
	// create fifo
	umask(0); // reset permission for next file
	if ( mkfifo(FIFO_NAME, 0666) == -1) // last type dev_t for device using
	{
		perror("error create fifo");
		exit(1);
	}
	for (int i = 0; i < 2; i++)
	{
		if( (pid = fork()) == 0) // child process
		{
			printf("{%d} [%d] -> [%d]\n", i, getppid(), getpid());
			//FILE *fp = fopen(FIFO_NAME, "w");
			int fp = open(FIFO_NAME, O_WRONLY);
			
			if (fp == 0)
				printf("error ");
			//fprintf(fp, "%s from %d \n", PING, getpid());
			
			char to_out[80];
			sprintf(to_out, "%s from %d \n", PING, getpid());
			write(fp, to_out, strlen(to_out) + 1);
			close(fp);
			//printf("string to send: %s\n", to_out);
			char tmp_buf[80];
			read(fp, tmp_buf, 80);
			printf("tmp: %s\n", tmp_buf);
			exit(1);
		}
		else
		{
			
			int statloc = 0;
			waitpid(pid, &statloc, 0);

			int fp_open = open(FIFO_NAME, O_RDONLY);
			if (fp_open != 0)
			{
				char receive_buf[80];

				/*fscanf(fp_open,"%s", receive_buf);
				strcat(receive_buf,"");*/
				read(fp_open, &receive_buf, 80);
				close(fp_open);
				
				printf("from child [%s]\n", receive_buf);
			}
			else 
				printf("{%d} error to open\n", i);
			
		}
	}
	/*FILE *fp = fopen(FIFO_NAME, "r");
	if (fp != NULL)
	{
	char receive_buf[80];

	fscanf(fp,"%s", receive_buf);
	strcat(receive_buf,"");
	fclose(fp);
	
	printf("from child %s\n", receive_buf);
	}
	else 
		printf("error\n");*/
	return 0;
}
