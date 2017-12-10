#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>
#include<sys/stat.h>
#include"tlpi_hdr.h"
#define BUF 10
int main(int ac, char*av[])
{
	int fd;
	ssize_t numrd;
	char buf[BUFSIZ-1];

	if (ac > 3 || ac < 2) {
		usageErr("tee (-a) filename");
	}
	fd = open(av[1], O_WRONLY | O_CREAT);
	if (fd == -1)
		errExit("open");
	
	while ((numrd=read(STDIN_FILENO,buf,BUF))!=-1) {
		write(fd, buf,numrd);
		printf("\n%d\n", numrd);
		fputs(buf, stdout);
	}
	return 0;

		
}