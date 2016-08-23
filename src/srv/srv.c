/*
 * Based on Dave O'Hallarons, Carnegie Mellon tiny.c
 * https://www.cs.cmu.edu/afs/cs/academic/class/15213-f08/www/code/tiny/
 */
#include <config.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "srv.h"
#include "mtab_check.h"

#define BUFSIZE 1024
#define MAXERRS 16

extern char **environ; /* the environment */

char *as = "501";

static int parentfd;          /* parent socket */
static int childfd;           /* child socket */


/*
 * error - wrapper for perror used for bad syscalls
 */
static void error(char *msg)
{
  perror(msg);
}

/*
 * cerror - returns an error message to the client
 */
static void cerror(FILE *stream, char *cause, char *err, char *shortmsg, char *longmsg
)
{
  fprintf(stream, "HTTP/1.1 %s %s\n", err, shortmsg);
  fprintf(stream, "Content-type: text/html\n");
  fprintf(stream, "\n");
  fprintf(stream, "<html><title>Tiny Error</title>");
  fprintf(stream, "<body bgcolor=""ffffff"">\n");
  fprintf(stream, "%s: %s\n", err, shortmsg);
  fprintf(stream, "<p>%s: %s\n", "sdf", cause);
  fprintf(stream, "<hr><em>The Tiny Web server</em>\n");
}


static int quit(pthread_mutex_t *mtx)
{
  switch(pthread_mutex_trylock(mtx)) {
    case 0: /* if we got the lock, unlock and return 1 (true) */
      pthread_mutex_unlock(mtx);
      return 1;
    case EBUSY: /* return 0 (false) if the mutex was locked */
      return 0;
  }
  return 1;
}
static pthread_mutex_t mxq; /* mutex used as quit flag */
static pthread_mutex_t socket_lock;
static void init_server(void)
{
//    pthread_t srv_thread;
//    init_server();
//    sigset_t set;
//    sigemptyset(&set);
//    sigaddset(&set, SIGPIPE);
//    pthread_sigmask(SIG_BLOCK, &set, NULL);
//    pthread_create(&srv_thread, NULL, &run_server, &mxq);
    pthread_mutex_init(&mxq,NULL);
    pthread_mutex_lock(&mxq);
    pthread_mutex_init(&socket_lock,NULL);

}

void* ST_start_server(void)
{
    //if not active
    init_server();

    /* variables for connection management */
    int portno = SERVER_PORT;            /* port to listen on */
    socklen_t  clientlen;         /* byte size of client's address */
    struct hostent *hostp; /* client host info */
    char *hostaddrp;       /* dotted decimal host addr string */
    int optval;            /* flag value for setsockopt */
    struct sockaddr_in serveraddr; /* server's addr */
    struct sockaddr_in clientaddr; /* client addr */

    /* variables for connection I/O */
    FILE *stream;          /* stream version of childfd */
    char buf[BUFSIZE];     /* message buffer */
    char method[BUFSIZE];  /* request method */
    char uri[BUFSIZE];     /* request uri */
    char version[BUFSIZE]; /* request method */


    /* open socket descriptor */
    parentfd = socket(AF_INET, SOCK_STREAM, 0);
    if (parentfd < 0)
    {
        error("ERROR opening socket");
    }

    /* allows us to restart server immediately */
    optval = 1;
    setsockopt(parentfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));

    /* bind port to socket */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    serveraddr.sin_port = htons((unsigned short)portno);
    if (bind(parentfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
    {
        error("ERROR on binding");
    }

    /* get us ready to accept connection requests */
    /* allow 5 requests to queue up */
    if (listen(parentfd, 5) < 0){
        error("ERROR on listen");
    }

    /*
     * main loop: wait for a connection request, parse HTTP,
     * serve requested content, close connection.
    */
    clientlen = sizeof(clientaddr);

    while(!quit(&mxq))
    {
        pthread_mutex_unlock(&socket_lock);
        /* wait for a connection request */
        childfd = accept(parentfd, (struct sockaddr *) &clientaddr, &clientlen);
        if (childfd < 0){
            error("ERROR on accept");
            break;
        }
        pthread_mutex_lock(&socket_lock);
        /* determine who sent the message */
        hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr), AF_INET);
        if (hostp == NULL){
            error("ERROR on gethostbyaddr");
        }
        hostaddrp = inet_ntoa(clientaddr.sin_addr);
        if (hostaddrp == NULL){
          error("ERROR on inet_ntoa\n");
        }

        /* open the child socket descriptor as a stream */
        if ((stream = fdopen(childfd, "r+")) == NULL){
            error("ERROR on fdopen");
        }
        /* get the HTTP request line */
        fgets(buf, BUFSIZE, stream);
        sscanf(buf, "%s %s %s\n", method, uri, version);

        if (strcasecmp(method, "GET"))
        {
            cerror(stream, method, "501", "Not Implemented", "Stored does not implement this method");
            fclose(stream);
            close(childfd);
            continue;
        }

        /* read (and ignore) the HTTP headers */
        fgets(buf, BUFSIZE, stream);
        while(strcmp(buf, "\r\n"))
        {
            fgets(buf, BUFSIZE, stream);
        }
        ST_report_list(stream);
        /* clean up */
        fclose(stream);
        close(childfd);
    }
    return NULL;
}


void ST_stop_server(void){
     pthread_mutex_lock(&socket_lock);
     shutdown(childfd, SHUT_RDWR);
     shutdown(parentfd, SHUT_RDWR);
     pthread_mutex_unlock(&socket_lock);
     pthread_mutex_unlock(&mxq);
//     pthread_join(srv_thread, NULL);
}