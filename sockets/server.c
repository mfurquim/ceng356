#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

#define BUF_SIZE 500

int
main(int argc, char *argv[])
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sfd, s;
	struct sockaddr_storage peer_addr;
	socklen_t peer_addr_len;
	ssize_t nread;
	float buf;

	float position;
	float number[2];
	struct sockaddr_storage peer_addr_player[2];
	int n_players;
	int buf_int;
	char* players[2];
	char last_service[NI_MAXSERV];
	
	position = 0;
	n_players = 0;
	players[0] = (char*) malloc(sizeof(char)*NI_MAXSERV);
	players[1] = (char*) malloc(sizeof(char)*NI_MAXSERV);


	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
	hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
	hints.ai_protocol = 0;          /* Any protocol */
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	s = getaddrinfo(NULL, "9966", &hints, &result);
	if (s != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		exit(EXIT_FAILURE);
	}

	/* getaddrinfo() returns a list of address structures.
	   Try each address until we successfully bind(2).
	   If socket(2) (or bind(2)) fails, we (close the socket
	   and) try the next address. */

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype,
				rp->ai_protocol);
		if (sfd == -1)
			continue;

		if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
			break;                  /* Success */

		close(sfd);
	}

	if (rp == NULL) {               /* No address succeeded */
		fprintf(stderr, "Could not bind\n");
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(result);           /* No longer needed */

	/* Read datagrams and store player service */

	for (;;) {
		if (n_players >= 2)
			break;

		peer_addr_len = sizeof(struct sockaddr_storage);
		nread = recvfrom(sfd, &buf, sizeof(float), 0,
				(struct sockaddr *) &peer_addr, &peer_addr_len);
		if (nread == -1)
			continue;               /* Ignore failed request */

		char host[NI_MAXHOST], service[NI_MAXSERV];

		s = getnameinfo((struct sockaddr *) &peer_addr,
						peer_addr_len, host, NI_MAXHOST,
						service, NI_MAXSERV, NI_NUMERICSERV);
		if (s == 0)
		{
			printf("Received %ld bytes from %s:%s\n", (long) nread, host, service);

			memcpy (players[n_players], service, strlen(service)+1);
			number[n_players] = (float) buf;
			peer_addr_player[n_players] = peer_addr;
			n_players += 1;
		}
		else
			fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));

		buf_int = n_players;
		printf("%d == %d\n", buf_int, n_players);
		if (sendto(sfd, &buf_int, sizeof(int), 0,
					(struct sockaddr *) &peer_addr,
					peer_addr_len) != sizeof(int))
			fprintf(stderr, "Error sending response\n");
	}

	printf("%s [%4.2f]\n", players[0], number[0]);
	printf("%s [%4.2f]\n", players[1], number[1]);

	buf = number[0] - number[1];
	if (sendto(sfd, &buf, nread, 0,
				(struct sockaddr *) &peer_addr_player[0],
				peer_addr_len) != nread)
		fprintf(stderr, "Error sending response\n");

	if (sendto(sfd, &buf, nread, 0,
				(struct sockaddr *) &peer_addr_player[1],
				peer_addr_len) != nread)
		fprintf(stderr, "Error sending response\n");
	n_players = 0;

	/* Main game loop'*/
	for (;;) {
		peer_addr_len = sizeof(struct sockaddr_storage);
		nread = recvfrom(sfd, &buf, sizeof(float), 0,
				(struct sockaddr *) &peer_addr, &peer_addr_len);
		if (nread == -1)
			continue;               /* Ignore failed request */

		char host[NI_MAXHOST], service[NI_MAXSERV];

		s = getnameinfo((struct sockaddr *) &peer_addr,
						peer_addr_len, host, NI_MAXHOST,
						service, NI_MAXSERV, NI_NUMERICSERV);
		if (s == 0)
		{
			/*
			*/
			printf("Received %ld bytes from %s:%s\n", (long) nread, host, service);
			if (n_players == 0)
			{
				if (strncmp (service, players[0], strlen(service)+1) == 0)
					number[0] = buf;
				else
					number[1] = buf;

				memcpy (last_service, service, strlen(service)+1);
				n_players += 1;
				continue;
			}

			if (strncmp (service, last_service, strlen(service)+1) == 0)
				continue;

			if (strncmp (service, players[0], strlen(service)+1) == 0)
				number[0] = buf;
			else
				number[1] = buf;

			n_players += 1;
		}
		else
			fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));

		if (n_players >= 2)
		{
			printf("%s [%4.2f]\n", players[0], number[0]);
			printf("%s [%4.2f]\n", players[1], number[1]);
			/*
			*/
			position += number[0] + number[1];
			buf = position;
			if (sendto(sfd, &buf, nread, 0,
						(struct sockaddr *) &peer_addr_player[0],
						peer_addr_len) != nread)
				fprintf(stderr, "Error sending response\n");

			if (sendto(sfd, &buf, nread, 0,
						(struct sockaddr *) &peer_addr_player[1],
						peer_addr_len) != nread)
				fprintf(stderr, "Error sending response\n");
			n_players = 0;
		}
	}
}