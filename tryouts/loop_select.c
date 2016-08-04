#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int main(void)
{
    fd_set rfds;
    int retval;
    char c;
    for(;;){
        FD_ZERO(&rfds);
        FD_SET(0, &rfds);
        retval = select(1, &rfds, NULL, NULL, NULL);
        if (retval){
            printf("Data is available now.\n");
         }
         while(EOF != fgetc(stdin));
    }
    exit(EXIT_SUCCESS);
}

