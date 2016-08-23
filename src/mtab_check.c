#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <mntent.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <sys/statvfs.h>
#include "mtab_check.h"
#include "util/logger.h"

typedef struct mntent M_TAB;

#define MSG_FMT "%s has %i percent space left."
#define MSG_FMT_LEN 30
#define MSG_LEN MSG_FMT_LEN + PATH_MAX

#define JSON_MSG_ROW_FMT "\"%s\": %i"
#define JSON_MSG_ROW_FMT_LEN 5

#define PERCENT_AND_COMMA_SIZE 5

#define JSON_MSG_HEADER_FMT "{\"entries\": {"
#define JSON_MSG_HEADER_FMT_LEN 14

#define JSON_MSG_FOOTER_FMT "}}"
#define JSON_MSG_FOOTER_FMT_LEN 3

#define INIT_MSG_BUFFER JSON_MSG_ROW_FMT_LEN + \
 PERCENT_AND_COMMA_SIZE + JSON_MSG_HEADER_FMT_LEN + \
 JSON_MSG_FOOTER_FMT_LEN

static bool active = false;
const static char msg_fmt[] = MSG_FMT;
static FILE* mtabf;
static struct statvfs s;
static char buf[MSG_LEN];

static char *msg_buf;
static char *msg_rows_buf;
static int runtime_msg_bufs_size =0;
static int entries_count =0;
static int runtime_entries_capacity;
static/*@only@*/ ST_NE *entries;

static void append_notice(int pos, char *path, int percent){
    pthread_mutex_lock(&ST_entries_lock);
    //Halving up
    if(entries_count == runtime_entries_capacity-1){
        ST_NE *tmp_entries;

        runtime_entries_capacity *=2;
        tmp_entries = (ST_NE* )realloc(entries, runtime_entries_capacity * sizeof(ST_NE));
        entries = tmp_entries;
        for(int i = entries_count; i < runtime_entries_capacity; i++)
        {
                entries[i] = NULL;
        }
    }

    if(NULL == entries[pos])
    {
        entries[pos] = malloc(sizeof(ST_NOTICED_ENTRY));
        strcpy (entries[pos]->path,path);
        entries[pos]->free_percent = percent;
    }
    else
    {
        strcpy(entries[pos]->path, path);
        entries[pos]->free_percent = percent;
    }
    pthread_mutex_unlock(&ST_entries_lock);
}

static void destory_current_notices(void){
    pthread_mutex_lock(&ST_entries_lock);
    for(int i = 0 ; i < runtime_entries_capacity; i++)
    {
        if(NULL != entries[i])
        {
            free(entries[i]);
            entries[i] = NULL;
        }
    }
    free(entries);
    pthread_mutex_unlock(&ST_entries_lock);
}

static void destroy_mtab(void)
{
    if( 0 == endmntent(mtabf)){
        ST_msg("endmntent fail", ST_MSG_ERROR);
    }
    destory_current_notices();
    free(msg_buf);
    free(msg_rows_buf);
    pthread_mutex_destroy(&ST_entries_lock);
}

static void init_mtab(void)
{
    atexit(&destroy_mtab);
    mtabf = setmntent(_PATH_MOUNTED, "r");
    if(!mtabf){
        ST_msg("setmntent fail", ST_MSG_ERROR);
    }
    runtime_entries_capacity = DEFAULT_NOTIFICATION_CAPACITY;
    entries = calloc(runtime_entries_capacity, sizeof(ST_NE));

    runtime_msg_bufs_size = INIT_MSG_BUFFER;
    msg_buf = calloc(runtime_msg_bufs_size, sizeof(char));
    msg_rows_buf = calloc(runtime_msg_bufs_size, sizeof(char));
    pthread_mutex_init(&ST_entries_lock, NULL);
}


static size_t approx_resp_buffers_size()
{
    size_t size = (JSON_MSG_ROW_FMT_LEN + PERCENT_AND_COMMA_SIZE) * entries_count;
    for(int i = 0; i < entries_count; i++)
    {
        size += strlen(entries[i]->path) + 1;
    }
    return size + JSON_MSG_HEADER_FMT_LEN + JSON_MSG_FOOTER_FMT_LEN;
}

void ST_report_list(FILE *stream)
{
    size_t row_len = 0;
    int msg_len = 0;
    pthread_mutex_lock(&ST_entries_lock);
    int buffer_approx = approx_resp_buffers_size();
    //Buffers halving
    char *tmp;
    if(buffer_approx > runtime_msg_bufs_size)
    {
        runtime_msg_bufs_size =2 * buffer_approx;
        tmp = (char *) realloc(msg_buf, runtime_msg_bufs_size * sizeof(char));
        msg_buf = tmp;
        tmp = (char *) realloc(msg_rows_buf, runtime_msg_bufs_size * sizeof(char));
        msg_rows_buf = tmp;
    }
    else if(buffer_approx <= (int) runtime_msg_bufs_size / 4)
    {
        runtime_msg_bufs_size /=2;
        tmp = (char *) realloc(msg_buf, runtime_msg_bufs_size * sizeof(char));
        msg_buf = tmp;
        tmp = (char *) realloc(msg_rows_buf, runtime_msg_bufs_size * sizeof(char));
        msg_rows_buf = tmp;
    }
    //clear buffers
    *msg_buf = 0;
    *msg_rows_buf = 0;
    strcat(msg_buf, JSON_MSG_HEADER_FMT);
    for(int i = 0; i < entries_count; i++)
    {
        row_len = sprintf(msg_rows_buf, JSON_MSG_ROW_FMT, entries[i]->path, entries[i]->free_percent);
        strncat(msg_buf, msg_rows_buf, row_len);
        if(i < entries_count - 1)
        {
           strcat(msg_buf, ",");
        }
    }
    strcat(msg_buf, JSON_MSG_FOOTER_FMT);
    msg_len = strlen(msg_buf);
    fprintf(stream, "HTTP/1.1 200 OK\n");
    fprintf(stream, "Server: stored daemon\n");
    fprintf(stream, "Content-length: %d\n", msg_len);
    fprintf(stream, "Content-type: %s\n", "application/json");
    fprintf(stream, "\r\n");
    fflush(stream);
    fwrite(msg_buf, 1, msg_len, stream);
    fflush(stream);
    pthread_mutex_unlock(&ST_entries_lock);
}


void ST_check_mtab(void)
{
    if(!active){
        active = true;
        init_mtab();
    }
    rewind(mtabf);
    M_TAB* mt;
    int free_percent;

    entries_count = 0;
    while((mt = getmntent(mtabf)))
    {
        if(0 == statvfs(mt->mnt_dir, &s))
        {
            if(0 < s.f_blocks)
            {
                free_percent = (int)(100.0 /((s.f_blocks - (s.f_bfree - s.f_bavail)) * s.f_bsize) * (s.f_bavail * s.f_bsize));
                if(FREE_PERCENT_NOTICE >= free_percent)
                {
                    append_notice(entries_count, mt->mnt_dir, free_percent);
                    entries_count++;
                    #ifndef IS_DAEMON
                        printf("event time: %u on: %s\n", (unsigned int)time(0), mt->mnt_dir);
                    #endif
                    sprintf(buf, msg_fmt, mt->mnt_dir, free_percent);
                    if(FREE_PERCENT_CRIT >= free_percent){
                        ST_msg(buf, ST_MSG_CRIT);
                    }
                     else if(FREE_PERCENT_WARN >= free_percent){
                        ST_msg(buf, ST_MSG_WARN);
                     }
                     else
                     {
                        ST_msg(buf, ST_MSG_NOTICE);
                     }
                }
            }
        }
        else
        {
            ST_msg("statvfs error", ST_MSG_ERROR);
        }
    }
    pthread_mutex_lock(&ST_entries_lock);
    //Halving down
    if(entries_count > 0 && entries_count == (int) (runtime_entries_capacity-1)/ 4)
    {
        ST_NE *tmp_entries;
        int old_capacity = runtime_entries_capacity;
        runtime_entries_capacity /=2;
        for(int i = runtime_entries_capacity; i < old_capacity; i++)
        {
            if(NULL != entries[i])
            {
                free(entries[i]);
                entries[i] = NULL;
            }
        }
        tmp_entries = (ST_NE* )realloc(entries, runtime_entries_capacity * sizeof(ST_NE));
        entries = tmp_entries;
    }
    pthread_mutex_unlock(&ST_entries_lock);
}

