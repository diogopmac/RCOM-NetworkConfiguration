#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>


#define FTP_PORT 21
#define MAX_LEN 1024

typedef struct {
    char username[MAX_LEN+1];
    char password[MAX_LEN+1];
    char domain[MAX_LEN+1];
    char path[MAX_LEN+1];
} Url;

// ftp://[<user>:<password>@]<host>/<url-path>

int decode_rfc1738(char *ftp_link, Url *ftp_url){
    if(strlen(ftp_link) > MAX_LEN){
        printf("URL maximum size exceeded\n");
        return -1;
    }

    char *schema = strtok(ftp_link, ":");
    if (schema == NULL) {
        printf("No schema provided.\n");
        return -1;
    }
    if(strcmp(schema, "ftp")) {
        printf("Wrong protocol selected.\nCorrect link format: ftp://[<user>:<password>@]<host>/<url-path>\n");
        return -1;
    }

    char *url = strtok(NULL, " ");
    if(url == NULL){
        printf("No URL provided\n");
        return -1;
    }
    if (url[0] != '/' || url[1] != '/'){
        printf("Wrong URL format.\nCorrect link format: ftp://[<user>:<password>@]<host>/<url-path>\n");
        return -1;
    }
    url = url+2;

    char *at = strchr(url, '@');
    char *start;
    if (at) {
        char *div = strchr(url, ':');

        if (div) { // username:password@url
            strncpy(ftp_url->username, url, div-url);
            strncpy(ftp_url->password, div+1, at-div-1);

            ftp_url->username[div-url] = '\0';
            ftp_url->password[at-div-1] = '\0';
        }
        else { // username@url
            strncpy(ftp_url->username, url, at-url);
            strcpy(ftp_url->password, "anonymous");

            ftp_url->username[at-url] = '\0';
        }

        start = at +1;
    }
    else { // url
        strcpy(ftp_url->username, "anonymous");
        strcpy(ftp_url->password, "anonymous");

        start = url;
    }

    if (strchr(start, ':')){
        printf("Bad URL.\n");
        return -1;
    }

    char *first_slash = strchr(start, '/');
    char *end = strchr(start, '\0');
    if (first_slash && 
        (first_slash-start > 0) )
    {
        strncpy(ftp_url->domain, start, first_slash-start);
        strncpy(ftp_url->path, first_slash+1, end-first_slash);

        ftp_url->domain[first_slash-start] = '\0';
        ftp_url->path[end-first_slash] = '\0';
    }
    else {
        printf("Bad URL\n");
        return -1;
    }

    return 0;
}

int socket_fd(const char *address, uint16_t port){
    int socket_fd = -1;
    struct sockaddr_in server_addr;

    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(address); 
    server_addr.sin_port = htons(port);      

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        exit(-1);
    }
    
    if (connect(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("connect()");
        exit(-1);
    }

    printf("Connected to %s:%d\n", address, port);
    return socket_fd;
}

int main(int argc, char **argv){
    if (argc < 2){
        printf("Usage: %s <ftp_url>\n", argv[0]);
        return -1;
    }

    Url url;

    if (decode_rfc1738(argv[1], &url) != 0){
        printf("ERROR: Couldn't parse URL.");
        return -1;
    }

    printf("Username : %s\n", url.username);
    printf("Password : %s\n", url.password);
    printf("Domain   : %s\n", url.domain);
    printf("Path     : %s\n", url.path);

    struct hostent *host = gethostbyname(url.domain);
    if (host == NULL){
        printf("ERROR: Host could not be found.");
        return -1;
    }

    const char *ip_address = inet_ntoa(*((struct in_addr *) host->h_addr));

    printf("Host name  : %s\n", host->h_name);
    printf("IP Address : %s\n", ip_address);

    int socket;
    printf("Connecting to %s\n", ip_address);
    socket = socket_fd(ip_address, FTP_PORT);

    printf("Socket File descriptor: %u\n", socket);

    return 0;
}
