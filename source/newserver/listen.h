#ifndef _LISTEN_H_
#define _LISTEN_H_

#include <sys/socket.h> // sockaddr_in
#include <netinet/in.h>	// sockaddr_in
#include <sys/poll.h>	// for pollfd in ufds
#include <pthread.h>	// pthread_t

#define MAX_IBUFFSIZE 128

struct connection {
        int connfd;
        struct pollfd ufds;
		void *player_info;		// this would be a pointer to the actual plr.
};

void init_listen_thread(pthread_t *tid);

int player_write(struct connection *cx, char *msg);
int player_read(struct connection *cx, char *dest, int maxsize);

#endif
