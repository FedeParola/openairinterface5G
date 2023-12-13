#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/in.h>
#include <netinet/sctp.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>       

#define REMOTE_ADDR 0x8446a8c0 /* 192.168.70.132 in nbo */
#define LISTEN_PORT 38412
#define BUFSIZE 8192
#define PPID 60

static volatile int stop = 0;

static void sigint_handler(int signum)
{
	stop = 1;
}

static void relay(int src, int dst)
{
	char buffer[BUFSIZE];
	int nrecv;

	nrecv = recv(src, buffer, BUFSIZE, 0);
	if (nrecv < 0) {
		fprintf(stderr, "Error receiving data: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (send(dst, buffer, nrecv, 0) != nrecv) {
		fprintf(stderr, "Error sending data: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	return;
}

int main(int argc, char *argv[])
{
	char buffer[BUFSIZE];
	int nrecv;

	int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s < 0) {
		fprintf(stderr, "Error creating local socket: %s\n",
			strerror(errno));
		exit(EXIT_FAILURE);
	}

	int val = 1;
	if (setsockopt(s, SOL_SOCKET, SO_REUSEPORT, &val, sizeof(val))) {
		fprintf(stderr, "Unable to set SO_REUSEPORT sockopt: %s\n",
			strerror(errno));
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(LISTEN_PORT);
	if (bind(s, (struct sockaddr *)&addr, sizeof(addr))) {
		fprintf(stderr, "Error binding local socket: %s\n",
			strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (listen(s, 16)) {
		fprintf(stderr, "Error listening on local socket: %s\n",
			strerror(errno));
		exit(EXIT_FAILURE);
	}

	printf("Waiting for local connection... ");
	fflush(stdout);

	int ls = accept(s, NULL, NULL);
	if (ls < 0) {
		fprintf(stderr, "Error accepting connection: %s\n",
			strerror(errno));
		exit(EXIT_FAILURE);
	}

	close(s);

	printf("Connected!\nConnecting to remote endpoint... ");

	int rs = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
	if (rs < 0) {
		fprintf(stderr, "Error creating remote socket: %s\n",
			strerror(errno));
		exit(EXIT_FAILURE);
	}

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = REMOTE_ADDR;
	addr.sin_port = htons(LISTEN_PORT);
	if (connect(rs, (struct sockaddr *)&addr, sizeof(addr))) {
		fprintf(stderr, "Error connecting to remote host: %s\n",
			strerror(errno));
		exit(EXIT_FAILURE);
	}

	printf("Connected!\n");

	if (fcntl(ls, F_SETFL, O_NONBLOCK)) {
		fprintf(stderr, "Error setting nonblocking local socket: %s\n",
			strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (fcntl(rs, F_SETFL, O_NONBLOCK)) {
		fprintf(stderr, "Error setting nonblocking remote socket: %s\n",
			strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (signal(SIGINT, sigint_handler) == SIG_ERR) {
		fprintf(stderr, "Error setting signal handler: %s\n",
			strerror(errno));
		exit(EXIT_FAILURE);
	}

	printf("Relaying messages...\n");

	struct pollfd pfds[2];

	pfds[0].fd = ls;
	pfds[0].events = POLLIN;
	pfds[1].fd = rs;
	pfds[1].events = POLLIN;

	while (!stop) {
		int ret = poll(pfds, 2, -1);
		if (ret <= 0) {
			if (errno == EINTR)
				continue;
			fprintf(stderr, "Error polling sockets %d: %s\n", ret,
				strerror(errno));
			exit(EXIT_FAILURE);
		}

		if (pfds[0].revents) {
			if (pfds[0].revents != POLLIN) {
				fprintf(stderr, "Unexpected event on local "
					"socket\n");
				exit(EXIT_FAILURE);	
			}

			nrecv = recv(ls, buffer, BUFSIZE, 0);
			if (nrecv < 0) {
				fprintf(stderr, "Error receiving data from "
					"local socket: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}

			if (sctp_sendmsg(rs, buffer, nrecv, NULL, 0,
					 htonl(PPID), 0, 0, 0, 0) != nrecv) {
				fprintf(stderr, "Error sending data on remote "
					"socket: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}

			printf("LOC -> REM: %d B\n", nrecv);
		}

		if (pfds[1].revents) {
			if (pfds[1].revents != POLLIN) {
				fprintf(stderr, "Unexpected event on remote "
					"socket\n");
				exit(EXIT_FAILURE);	
			}

			nrecv = recv(rs, buffer, BUFSIZE, 0);
			if (nrecv < 0) {
				fprintf(stderr, "Error receiving data from "
					"remote socket: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}

			if (send(ls, buffer, nrecv, 0) != nrecv) {
				fprintf(stderr, "Error sending data on local "
					"socket: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}

			printf("REM -> LOC: %d B\n", nrecv);
		}
	}

	close(ls);
	close(rs);

	return 0;
}