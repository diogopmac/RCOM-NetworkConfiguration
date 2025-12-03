#include <stdio.h>
#include <string.h>

#define FTP_PORT 21
#define MAX_LEN 1024

char username[MAX_LEN];
char password[MAX_LEN];
char domain[MAX_LEN];
char path[MAX_LEN];

// ftp://[<user>:<password>@]<host>/<url-path>

int decode_rfc1738(char *ftp_link){
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
            strncpy(username, url, div-url);
            strncpy(password, div+1, at-div-1);

            username[div-url] = '\0';
            password[at-div-1] = '\0';
        }
        else { // username@url
            strncpy(username, url, at-url);
            strcpy(password, "anonymous");

            username[at-url] = '\0';
        }

        start = at +1;
    }
    else { // url
        strcpy(username, "anonymous");
        strcpy(password, "anonymous");

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
        strncpy(domain, start, first_slash-start);
        strncpy(path, first_slash+1, end-first_slash);

        domain[first_slash-start] = '\0';
        path[end-first_slash] = '\0';
    }
    else {
        printf("Bad URL\n");
        return -1;
    }

    printf("Username: %s\n", username);
    printf("Password: %s\n", password);
    printf("Domain: %s\n", domain);
    printf("Path: %s\n", path);

    return 0;
}

int main(int argc, char **argv){
    if (argc < 2){
        printf("Usage: %s <ftp_url>\n", argv[0]);
        return -1;
    }

    if (decode_rfc1738(argv[1]) != 0){
        printf("Couldn't parse URL.");
        return -1;
    }

    return 0;
}
