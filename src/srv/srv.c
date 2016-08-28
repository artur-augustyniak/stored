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

static int parentfd;
static int childfd;
static pthread_mutex_t mxq;
static pthread_mutex_t socket_lock;
static pthread_t srv_thread;
static sigset_t set;
static in_addr_t address;

int ST_enabled = SERVER_ENABLED;
int ST_port = SERVER_PORT;
const char *ST_bind_addr = BIND_ADDR;

static void error(char *msg)
{
  perror(msg);
  exit(1);
}

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

static void* serve(void* none)
{
    /* variables for connection management */
    int portno = ST_port;            /* port to listen on */
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
    serveraddr.sin_addr.s_addr = address;
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

        fclose(stream);
        close(childfd);
    }
    return NULL;
}

void ST_start_server(void)
{
    //if not active
    address = inet_addr(ST_bind_addr);

//    sigemptyset(&set);
//    sigaddset(&set, SIGPIPE);
//    sigaddset(&set, SIGHUP);
//    pthread_sigmask(SIG_BLOCK, &set, NULL);

    pthread_mutex_init(&mxq,NULL);
    pthread_mutex_lock(&mxq);
    pthread_mutex_init(&socket_lock,NULL);
    pthread_create(&srv_thread, NULL, &serve, &mxq);
}

void ST_stop_server(void){
     pthread_mutex_lock(&socket_lock);
     shutdown(childfd, SHUT_RDWR);
     shutdown(parentfd, SHUT_RDWR);
     pthread_mutex_unlock(&mxq);
     pthread_mutex_unlock(&socket_lock);
     pthread_join(srv_thread, NULL);
}