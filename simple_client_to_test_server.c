/*
*gcc simple_client_to_test_server.c  -o simple_client_to_test_server
*./simple_client_to_test_server  192.168.10.63 40713 
*/
#include <netinet/in.h>/* For sockaddr_in */
#include <sys/socket.h>/* For socket functions */

#include <unistd.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	struct sockaddr_in sin;
	int fd;

	const char *cp;
	ssize_t n_written, remaining;
	char sendbuf[1024] = "hello server!\n";
	char recvbuf[1024];

	if (argc != 3)
	{
		fprintf(stderr,"usage: %s ipaddr port\n",argv[0]);
		return 1;
	}

	/* Allocate a new socket */
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		perror("socket");
		return 1;
	}

	/* Connect to the remote host. */
	sin.sin_family = AF_INET;
	inet_aton(argv[1],&sin.sin_addr);
	sin.sin_port = htons(atoi(argv[2]));
	if (connect(fd, (struct sockaddr*) &sin, sizeof(sin))) {
		perror("connect");
		close(fd);
		return 1;
	}

	cp = sendbuf;
	remaining = strlen(sendbuf);
	while (remaining) {
		n_written = send(fd, cp, remaining, 0);
		if (n_written <= 0) {
			perror("send");
			return 1;
		}
		remaining -= n_written;
		cp += n_written;
	}

	/* Get an answer back. */
	while (1) {
		ssize_t result = recv(fd, recvbuf, sizeof(recvbuf), 0);
		if (result == 0) {
			break; /*receive all msg*/
		} else if (result < 0) {
			perror("recv");
			close(fd);
			return 1;
		}
		fwrite(recvbuf, 1, result, stdout);
	}

	close(fd);
	return 0;
}
