#include <stdio.h>
#include <string.h>

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
        (first_slash-start > 0) 
    ){
        strncpy(ftp_url->domain, start, first_slash-start);
        strncpy(ftp_url->path, first_slash+1, end-first_slash);

        ftp_url->domain[first_slash-start] = '\0';
        ftp_url->path[end-first_slash] = '\0';
    }
    else {
        printf("Bad URL\n");
        return -1;
    }

    printf("Username: %s\n", ftp_url->username);
    printf("Password: %s\n", ftp_url->password);
    printf("Domain: %s\n", ftp_url->domain);
    printf("Path: %s\n", ftp_url->path);

    return 0;
}

int main(int argc, char **argv){
    if (argc < 2){
        printf("Usage: %s <ftp_url>\n", argv[0]);
        return -1;
    }

    Url url;

    if (decode_rfc1738(argv[1], &url) != 0){
        printf("Couldn't parse URL.");
        return -1;
    }

    return 0;
}
