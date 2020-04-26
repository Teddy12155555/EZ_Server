all: Http_Server Post Get

CC = gcc
Host: host.c
	$(CC) -o Server Http_Server.c

Post: POST_CGI.c
	$(CC) -o ./src/postCGI.cgi POST_CGI.c

Get: GET_CGI.c
	$(CC) -o ./src/getCGI.cgi GET_CGI.c

.PHONY: clean,run
clean:
	rm Host Post.cgi Get.cgi

test:
	./Server
