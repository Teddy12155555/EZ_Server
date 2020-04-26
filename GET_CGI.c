#include <stdio.h>
#include <unistd.h>
#include <mach/error.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>
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

    FILE *pFile;
    pFile = fopen("DB.txt", "r");
    char read_buffer[1024];
    if (NULL == pFile)
    {
        printf("Open failure");
        printf("<HTML><HEAD><meta http-equiv=\"content-type\" content=\"text/html;charset=utf-8\">\n");
        printf("<TITLE>GET CGI </TITLE>\n");
        printf("<BODY><h1>Simple Server with Get,Post by Teddy</h1>\n<form action=\"\" method=\"POST\"><h1>Your Num is : </h1><br />\n<label for=\"to\">Add NUM</label>\n<input name=\"num\" >\n<button>Write into DB</button>\n</form></BODY>");
        printf("</HTML>\n");
        return 1;
    }
    else
    {
        fgets(read_buffer, 1024, pFile);
        fclose(pFile);
        // output to stdout
        printf("<HTML><HEAD><meta http-equiv=\"content-type\" content=\"text/html;charset=utf-8\">\n");
        printf("<TITLE>GET CGI </TITLE>\n");
        printf("<BODY><h1>Simple Server with Get,Post by Teddy</h1>\n<form action=\"\" method=\"POST\"><h1>Your Num is : </h1><br />\n<label for=\"to\">Add NUM</label>\n<input name=\"num\" >\n<button>Write into DB</button>\n</form></BODY>");
        printf("DataBase : %s ", read_buffer);
        printf("</HTML>\n");
        memset(read_buffer, 0, 1024);
    }
}