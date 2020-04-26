#include <stdio.h>
#include <unistd.h>
#include <mach/error.h>
#include <stdlib.h>
#include <sys/ioctl.h>
/*
Please make sure you understand host.c
*/
int main(void)
{
    int unread;
    char *buf;

    // wait for stdin
    while (unread < 1)
    {
        if (ioctl(STDIN_FILENO, FIONREAD, &unread))
        {
            perror("ioctl");
            exit(EXIT_FAILURE);
        }
    }
    buf = (char *)malloc(sizeof(char) * (unread + 1));

    // read from stdin fd
    read(STDIN_FILENO, buf, unread);
    int num = atoi(buf);

    FILE *pFile;
    pFile = fopen("DB.txt", "a");
    fprintf(pFile, "%d ", num);
    fclose(pFile);

    // output to stdout
    printf("<HTML><HEAD><meta http-equiv=\"content-type\" content=\"text/html;charset=utf-8\">\n");
    printf("<TITLE>CGI POST </TITLE>\n");
    printf("<BODY>Num: %s is write to DataBase</BODY></HTML>\n", buf);
}
