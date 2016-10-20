//#include <config.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "srv.h"
#include "common.h"

#define BUFSIZE 1024

static ST_CONFIG config;
static int parentfd;
static int childfd;
static pthread_mutex_t mxq;
static in_addr_t address;
static  pthread_t srv_thread;
static ST_SRV_BUFF srv_buffer;
static char *content_buffer = NULL;
static bool stopped = true;
static sigset_t set;

static void cerror(FILE *stream, char *cause, char *err, char *shortmsg, char *longmsg)
{
  fprintf(stream, "HTTP/1.1 %s %s\n", err, shortmsg);
  fprintf(stream, "Content-type: text/html\n");
  fprintf(stream, "\n");
  fprintf(stream, "<html><title>Stored error</title>");
  fprintf(stream, "<body bgcolor=""ffffff"">\n");
  fprintf(stream, "%s: %s\n", err, shortmsg);
  fprintf(stream, "<p>%s: %s\n", "blah", cause);
  fprintf(stream, "<hr><em>(_|_)</em>\n");
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

    sigemptyset(&set);
    sigaddset(&set, SIGPIPE);
    pthread_sigmask(SIG_BLOCK, &set, NULL);

    /* variables for connection management */
    ST_lock(&config->mutex);
    int portno = config->http_port;            /* port to listen on */
    ST_unlock(&config->mutex);
    socklen_t  clientlen;         /* byte size of client's address */
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
        ST_abort(
            __FILE__,
            __LINE__,
            "ERROR opening socket"
        );
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
        ST_abort(
            __FILE__,
            __LINE__,
            "ERROR on binding"
        );
    }

    /* get us ready to accept connection requests */
    /* allow 5 requests to queue up */
    if (listen(parentfd, 5) < 0)
    {
        ST_abort(
            __FILE__,
            __LINE__,
            "ERROR on listen."
        );
    }

    /*
     * main loop: wait for a connection request, parse HTTP,
     * serve requested content, close connection.
    */
    clientlen = sizeof(clientaddr);

    while(!quit(&mxq))
    {

        /* wait for a connection request */
        childfd = accept(parentfd, (struct sockaddr *) &clientaddr, &clientlen);
        if (childfd < 0){
            //error("ERROR on accept");
            break;
        }
        
        hostaddrp = inet_ntoa(clientaddr.sin_addr);
        if (hostaddrp == NULL)
        {
            ST_abort(
                __FILE__,
                __LINE__,
                "ERROR on inet_ntoa"
            );
        }

        /* open the child socket descriptor as a stream */
        if ((stream = fdopen(childfd, "r+")) == NULL)
        {
            ST_abort(
                __FILE__,
                __LINE__,
                "ERROR on fdopen"
            );
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

        ST_lock(&srv_buffer->mutex);
        if(!srv_buffer->data)
        {
            ST_abort(
                __FILE__,
                __LINE__,
                "Srv buffer empty, please provide initial data"
            );
        }

        content_buffer = strdup(srv_buffer->data);
        ST_unlock(&srv_buffer->mutex);
        int len = strlen(content_buffer);
        fprintf(stream, "HTTP/1.1 200 OK\n");
        fprintf(stream, "Server: stored daemon\n");
        fprintf(stream, "Content-length: %d\n", len);
        fprintf(stream, "Content-type: %s\n", "text/json");
        fprintf(stream, "\r\n");
        //fflush(stream);
        fwrite(content_buffer, 1, len, stream);
        fflush(stream);
        fclose(stream);
        close(childfd);
        if(content_buffer)
        {
            free(content_buffer);
        }
    }
    return NULL;
}

ST_SRV_BUFF ST_init_srv(ST_CONFIG c)
{
    if(!c)
    {
        ST_abort(
            __FILE__,
            __LINE__,
            "ST_CONFIG (c) NPE"
        );
    }

    config = c;
    int cb_pthread_stat;
    ST_SRV_BUFF content_buffer;
    content_buffer = (ST_SRV_BUFF) malloc(sizeof(ST_SERVER_BUFFER));
    
    if(!content_buffer)
    {
        ST_abort(
            __FILE__,
            __LINE__,
            "OOM error. Exiting!"
        );
    }

    cb_pthread_stat = pthread_mutex_init(&content_buffer->mutex, NULL);
    if(cb_pthread_stat)
    {
        free(content_buffer);
        ST_abort(
            __FILE__,
            __LINE__,
            strerror(cb_pthread_stat)
        );
    }


    srv_buffer = content_buffer;
    return content_buffer;
}

void ST_start_srv(ST_SRV_BUFF b)
{
    if(!b)
    {
        ST_abort(
            __FILE__,
            __LINE__,
            "Srv start (b) NPE"
        );
    }
    int enabled = 0;
    ST_lock(&config->mutex);
    enabled = config->http_enabled;
    ST_unlock(&config->mutex);
    if(stopped && enabled)
    {
        int q_pthread_stat = pthread_mutex_init(&mxq,NULL);
        if(q_pthread_stat)
        {
            pthread_mutex_destroy(&b->mutex);
            free(b);
            ST_abort(
                __FILE__,
                __LINE__,
                strerror(q_pthread_stat)
             );
        }

        ST_lock(&config->mutex);
        address = inet_addr(config->http_bind_address);
        ST_unlock(&config->mutex);

        if(INADDR_NONE == address)
        {
            pthread_mutex_destroy(&mxq);
            pthread_mutex_destroy(&b->mutex);
            free(b);
            ST_abort(
                __FILE__,
                __LINE__,
                "Incorrect bind addr!"
            );
        }

        ST_lock(&mxq);
        pthread_create(&srv_thread, NULL, &serve, &mxq);
        stopped = false;
    }
}

void ST_stop_srv(ST_SRV_BUFF b)
{
    if(!stopped)
    {
        shutdown(childfd, SHUT_RDWR);
        shutdown(parentfd, SHUT_RDWR);
        pthread_mutex_unlock(&mxq);
        pthread_join(srv_thread, NULL);
        stopped = true;
    }
}

void ST_restart_srv(ST_SRV_BUFF b)
{
    ST_stop_srv(b);
    ST_start_srv(b);

}

void ST_destroy_srv(ST_SRV_BUFF b)
{
    ST_stop_srv(b);
    pthread_mutex_destroy(&mxq);
    pthread_mutex_destroy(&b->mutex);
    free(b);
}