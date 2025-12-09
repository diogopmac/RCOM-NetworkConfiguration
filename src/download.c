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

int message(int socket_fd, char *code){
    char c;
    char line[MAX_LEN];
    memset(line, 0, sizeof(line));
    int code_index = 0;
    int line_index = 0;
    int state = 0; // 0 code, 1 content, 2 final
    
    while (1) {
        read(socket_fd, &c, 1);

        switch(state){
            case 0:
                if (c == '-'){
                    line[line_index++] = c;
                    state = 1;
                }
                else if (c == ' '){
                    line[line_index++] = c;
                    code[3] = '\0';
                    state = 2;
                }
                else code[code_index++] = c;
                break;
            case 1:
                if (c == '\n'){
                    printf("%s%s\n", code, line);

                    memset(line, 0, sizeof(line));
                    memset(code, 0, MAX_LEN);

                    code_index = 0;
                    line_index = 0;

                    state = 0;
                    break;
                }
                else {
                    line[line_index++] = c;
                    break;
                }
            case 2:
                if (c == '\n'){
                    printf("%s%s\n", code, line);
                    memset(line, 0, sizeof line);
                    return 0;
                }
                else {
                    line[line_index++] = c;
                    break;
                }
                break;
        }
    }
    return -1;
}

int command(int socket_fd, char *command, char *content){
    if (write(socket_fd, command, strlen(command)) < 0) return -1;
    if (write(socket_fd, content, strlen(content)) < 0) return -1;
    if (write(socket_fd, "\r\n", 2) < 0) return -1;
    return 0;
}

int single_command(int socket_fd, char *command){
    if (write(socket_fd, command, strlen(command)) < 0) return -1;
    if (write(socket_fd, "\r\n", 2) < 0) return -1;
    return 0;
}

int code(char *code, char *expected){
    return strncmp(code, expected, strlen(expected));
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
    printf("Socket File descriptor: %u\n\nAttempting Connection...\n\n", socket);

    char response_code[MAX_LEN];
    if (message(socket, response_code) != 0){
        printf("Unable to connect to %s", ip_address);
        return -1;
    }
    else printf("\nConnection Successful.\nStarting Authentication.\n");

    printf("Sending USER %s\n", url.username);
    if(command(socket, "USER ", url.username) != 0
        || message(socket, response_code) != 0
        || code(response_code, "331") != 0){
        printf("Could not send command. Aborting.\n");
        return -1;
    }

    printf("Sending PASS %s\n", url.password);
    if(command(socket, "PASS ", url.password) != 0
        || message(socket, response_code) != 0
        || code(response_code, "230") != 0){
        printf("Could not send command. Aborting.\n");
        return -1;
    }

    printf("Sending QUIT\n");
    if(single_command(socket, "QUIT") != 0
        || message(socket, response_code) != 0
        || code(response_code, "221") != 0){
        printf("Could not send command. Aborting.\n");
        return -1;
    }

    printf("Closing socket %u\n", socket);
    if (close(socket) != 0){
        printf("Could not close socket.\n");
        return -1;
    }

    printf("Socket closed. Protocol Complete.\n");

    return 0;
}
