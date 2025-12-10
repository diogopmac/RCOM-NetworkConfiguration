#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>


#define FTP_PORT 21
#define MAX_LEN 1024
#define TRANSFER_LEN 2048

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

int open_socket(const char *address, uint16_t port){
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

int close_socket(const int socket_fd){
    if (close(socket_fd) < 0){
        perror("close()");
        exit(-1);
    }
    return 0;
}

int message(int socket_fd, char *code, char *content){
    char c;
    char line[MAX_LEN];
    memset(line, 0, sizeof(line));
    int code_index = 0;
    int line_index = 0;
    int state = 0; // 0 code, 1 multi-line, 2 single-line, 3 escape
    
    while (1) {
        ssize_t n = read(socket_fd, &c, 1);
        if (n == 0) {
            fprintf(stderr, "Connection closed by peer\n");
            return -1;
        } else if (n < 0) {
            perror("read()");
            return -1;
        }

        switch(state){
            case 0:
                if (code_index < 3) {
                    code[code_index++] = c;
                }
                else if (c == '-'){
                    if (line_index < MAX_LEN - 1) line[line_index++] = c;
                    code[3] = '\0';
                    state = 1;
                }
                else if (c == ' '){
                    if (line_index < MAX_LEN - 1) line[line_index++] = c;
                    code[3] = '\0';
                    state = 2;
                }
                break;
            case 1:
                if (c == '\n'){
                    state = 3;
                }
                else {
                    if (line_index < MAX_LEN - 1) line[line_index++] = c;
                }
                break;
            case 2:
                if (c == '\n'){
                    strncpy(content, line + 1, MAX_LEN-1);
                    memset(line, 0, sizeof line);
                    return 0;
                }
                else {
                    if (line_index < MAX_LEN - 1) line[line_index++] = c;
                    break;
                }
                break;
            case 3: 
                if (isdigit(c)){
                    printf("%s%s\n", code, line);

                    memset(line, 0, sizeof(line));
                    memset(code, 0, MAX_LEN);

                    code_index = 1;
                    line_index = 0;

                    code[0] = c;

                    state = 0;
                }
                else {
                    if (line_index < MAX_LEN - 1) line[line_index++] = c;
                    state = 1;
                }
        }
    }
    return -1;
}

int pasv_decode(int socket_fd, char *code, char *address, uint8_t port[2]){
    char c;
    char line[MAX_LEN];
    memset(line, 0, MAX_LEN);
    int line_index = 0;

    while (1){
        ssize_t n = read(socket_fd, &c, 1);
        if (n == 0) {
            fprintf(stderr, "Connection closed by peer\n");
            return -1;
        } else if (n < 0) {
            perror("read()");
            return -1;
        }

        if (line_index < MAX_LEN - 1) line[line_index++] = c;

        if (c == '\n') {
            line[line_index] = '\0';
            break;
        }
    }

    strncpy(code, line, 3);
    code[3] = '\0';

    int h1, h2, h3, h4, p1, p2;
    if (sscanf(line, "%*d %*[^(](%d,%d,%d,%d,%d,%d)", &h1, &h2, &h3, &h4, &p1, &p2) == 6) {
        sprintf(address, "%d.%d.%d.%d", h1, h2, h3, h4);
        port[0] = p1;
        port[1] = p2;
        return 0;
    } else {
        fprintf(stderr, "Failed to parse PASV response\n");
        return -1;
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

int check_response_code(char *code, char *expected){
    return strncmp(code, expected, strlen(expected));
}

int main(int argc, char **argv){
    if (argc < 2){
        printf("Usage: %s <ftp_url>\n", argv[0]);
        return -1;
    }

    Url url;

    if (decode_rfc1738(argv[1], &url) != 0){
        printf("ERROR: Couldn't parse URL.\n");
        return -1;
    }

    printf("Username : %s\n", url.username);
    printf("Password : %s\n", url.password);
    printf("Domain   : %s\n", url.domain);
    printf("Path     : %s\n", url.path);

    struct hostent *host = gethostbyname(url.domain);
    if (host == NULL){
        printf("ERROR: Host could not be found.\n");
        return -1;
    }

    const char *ip_address = inet_ntoa(*((struct in_addr *) host->h_addr));

    printf("Host name  : %s\n", host->h_name);
    printf("IP Address : %s\n", ip_address);

    int socket;
    printf("Connecting to %s\n", ip_address);

    socket = open_socket(ip_address, FTP_PORT);
    printf("Socket File descriptor: %d\n\nAttempting Connection...\n\n", socket);

    char response_code[MAX_LEN], content[MAX_LEN];
    if (message(socket, response_code, content) != 0){
        printf("Unable to connect to %s\n", ip_address);
        close(socket);
        return -1;
    }
    else printf("\nConnection Successful.\nStarting Authentication.\n\n");

    printf("Sending USER %s\n", url.username);
    if(command(socket, "USER ", url.username) != 0
        || message(socket, response_code, content) != 0
        || check_response_code(response_code, "331") != 0){
        printf("Could not send command. Aborting.\n");
        close_socket(socket);
        return -1;
    }
    printf("> %s %s\n\n", response_code, content);

    printf("Sending PASS %s\n", url.password);
    if(command(socket, "PASS ", url.password) != 0
        || message(socket, response_code, content) != 0
        || check_response_code(response_code, "230") != 0){
        printf("Could not send command. Aborting.\n");
        close_socket(socket);
        return -1;
    }
    printf("> %s %s\n\n", response_code, content);

    printf("Sending TYPE I\n");
    if(single_command(socket, "TYPE I") !=0 
        || message(socket, response_code, content) != 0
        || check_response_code(response_code, "200") != 0){
        printf("Could not enter Binary mode. Aborting\n");
        close_socket(socket);
        return -1;
    }
    printf("> %s %s\n\n", response_code, content);

    printf("Sending SIZE %s\n", url.path);
    if(command(socket, "SIZE ", url.path) != 0
        || message(socket, response_code, content) != 0
        || check_response_code(response_code, "213") != 0){
        printf("Could not send command. Aborting.\n");
        close_socket(socket);
        return -1;
    }
    printf("> %s %s\n\n", response_code, content);

    long file_size = atol(content);
    printf("File Size: %ld bytes\n", file_size);

    char address[16];
    uint8_t port[2];
    printf("Sending PASV\n");
    if(single_command(socket, "PASV") !=0 
        || pasv_decode(socket, response_code, address, port) != 0
        || check_response_code(response_code, "227") != 0){
        printf("Could not enter Binary mode. Aborting\n");
        close_socket(socket);
        return -1;
    }
    printf("> %s %s\n\n", response_code, content);

    int data_socket = open_socket(address, port[0]*256+port[1]);

    char *filename = strrchr(url.path, '/');
    if (filename) filename++;
    else filename = url.path;

    int file = open(filename, O_WRONLY | O_CREAT, 0640);

    uint8_t buffer[TRANSFER_LEN];
    size_t bytes_read = 0, total = 0;

    printf("Sending RETR %s\n", url.path);
    if(command(socket, "RETR ", url.path) != 0
        || message(socket, response_code, content) != 0
        || (check_response_code(response_code, "150") != 0
        && check_response_code(response_code, "125") != 0)){
        printf("Could not send command. Aborting.\n");
        close_socket(socket);
        return -1;
    }
    printf("> %s %s\n\n", response_code, content);

    while((bytes_read = read(data_socket, buffer, MAX_LEN)) > 0){
        if (write(file, buffer, bytes_read) < 0) {
            perror("write()");
            break;
        }

        total += bytes_read;
        printf("\rProgress: %d%% (%zu / %ld)", (int)((total * 100) / file_size), total, file_size);
        fflush(stdout);
    }
    printf("\n\n");
    close(file);

    printf("Transfer complete. Closing Data Socket %d\n", socket);
    close_socket(data_socket);

    if(message(socket, response_code, content) != 0
        || check_response_code(response_code, "226") != 0){
        printf("Download Failed.\n");
        close_socket(socket);
        return -1;
    }
    printf("> %s %s\n\n", response_code, content);

    printf("Sending QUIT\n");
    if(single_command(socket, "QUIT") != 0
        || message(socket, response_code, content) != 0
        || check_response_code(response_code, "221") != 0){
        printf("Could not send command. Aborting.\n");
        close_socket(socket);
        return -1;
    }
    printf("> %s %s\n\n", response_code, content);

    printf("Closing Control Socket %d\n", socket);
    close_socket(socket);

    printf("Socket closed. Protocol Complete.\n");

    return 0;
}
