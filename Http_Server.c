#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <dirent.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>

#define BUFSIZE 4096

void CallCGI(char *inputData, char *path, int fd, int isGet, int num)
{
    int cgiInput[2];
    int cgiOutput[2];
    int status;
    pid_t cpid;
    char c;
    if (pipe(cgiInput) < 0)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    if (pipe(cgiOutput) < 0)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    write(fd, "HTTP/1.0 200 OK\r\n", 17);
    write(fd, "Content-Type: text/html\r\n\r\n", 27);
    if ((cpid = fork()) < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    /*child process*/
    if (cpid == 0)
    {
        close(cgiInput[1]);
        close(cgiOutput[0]);
        dup2(cgiOutput[1], STDOUT_FILENO);
        dup2(cgiInput[0], STDIN_FILENO);
        close(cgiInput[0]);
        close(cgiOutput[1]);
        execlp(path, path, NULL);
        exit(0);
    }
    else
    {
        close(cgiOutput[1]);
        close(cgiInput[0]);

        if (isGet)
        {
            write(cgiInput[1], inputData, strlen(inputData));
        }
        else
        {
            char t[2048];
            sprintf(t, "%d", num);
            write(cgiInput[1], t, strlen(inputData));
        }
        while (read(cgiOutput[0], &c, 1) > 0)
            write(fd, &c, 1);

        send(STDOUT_FILENO, "\n", 1, 0);

        close(cgiOutput[0]);
        close(cgiInput[1]);
        waitpid(cpid, &status, 0);
    }
}

void Socket(int fd)
{
    int j, file_fd, buflen, len;
    long i, ret;
    char *fstr;
    static char buffer[BUFSIZE + 1];
    static char method[20];
    ret = read(fd, buffer, BUFSIZE);
    if (ret == 0 || ret == -1)
        exit(3);

    if (ret > 0 && ret < BUFSIZE) //make last be 0
        buffer[ret] = 0;
    else
        buffer[0] = 0;

    int k = 0;
    while (buffer[k] != 0 && buffer[k] != '/' && k < ret)
    {
        method[k] = buffer[k];
        k++;
    }
    if (strncmp(method, "GET ", 4) == 0)
    {
        write(fd, "HTTP/1.0 200 OK\r\n", 17);
        CallCGI("Hello", "./getCGI.cgi", fd, 1, 0);
    }
    else if (strncmp(method, "POST ", 5) == 0)
    {

        write(fd, "HTTP/1.0 200 OK\r\n", 17);
        int number = 0;
        for (i = 0; (i < ret) && (i + 4 <= ret); i++)
            if (buffer[i] == 'n' && buffer[i + 1] == 'u' && buffer[i + 2] == 'm' && buffer[i + 3] == '=')
            {
                i += 4;
                char temp[2048];
                int l = 0;
                while (buffer[i] != 0 && buffer[i] != '\n' && buffer[i] != '\r')
                {
                    temp[l] = buffer[i];
                    l++;
                    i++;
                }
                temp[l++] = 0;
                number = atoi(temp);
                printf("Debug : %d\n", number);
            }
        CallCGI("Hello", "./postCGI.cgi", fd, 0, number);
    }
    else
    {
        write(fd, "HTTP/1.0 404 NOTFOUND\r\n", 23);
        exit(3);
    }
    exit(1);
}

int main(int argc, char *argv[])
{
    in_addr_t ip;
    struct sockaddr_in socket_;
    static struct sockaddr_in cli_addr;
    int i, pid, listenfd, socketfd;
    size_t length;
    if (argc < 3)
    {
        printf("Please input format :[ipv4] [port]");
    }
    // open socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    //
    if (listenfd == -1)
    {
        fprintf(stderr, "Socket Error\n");
        exit(EXIT_FAILURE);
    }
    int TRUE_ = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &TRUE_, sizeof(int)) == -1)
    {
        fprintf(stderr, "Setsockopt Error\n");
        exit(EXIT_FAILURE);
    }
    bzero(&socket_, sizeof(socket_));
    socket_.sin_family = PF_INET;
    socket_.sin_addr.s_addr = ip = inet_addr(argv[1]);
    socket_.sin_port = htons(atoi(argv[2]));
    if (bind(listenfd, (struct sockaddr *)&socket_, sizeof(socket_)) == -1)
    {
        fprintf(stderr, "bind error\n");
        exit(EXIT_FAILURE);
    }
    if (listen(listenfd, 5))
    {
        fprintf(stderr, "listen error\n");
        exit(EXIT_FAILURE);
    }

    if (chdir("./src") == -1)
        exit(4);

    if (fork() != 0)
        return 0;

    signal(SIGCHLD, SIG_IGN);

    while (1)
    {
        length = sizeof(cli_addr);
        if ((socketfd = accept(listenfd, (struct sockaddr *)&cli_addr, &length)) < 0)
            exit(3);

        if ((pid = fork()) < 0)
            exit(3);
        else
        {
            if (pid == 0)
            {
                close(listenfd);
                Socket(socketfd);
            }
            else
            {
                close(socketfd);
            }
        }
    }

    return 0;
}
