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
        printf("URL maximum size exceeded");
        return -1;
    }

    char *schema = strtok(ftp_link, ":");

    printf("Schema: %s\n", schema);
    if (schema == NULL) {
        printf("No schema provided.\n");
        return -1;
    }
    if(strcmp(schema, "ftp")) {
        printf("Wrong protocol selected.\nCorrect link format: ftp://[<user>:<password>@]<host>/<url-path>");
        return -1;
    }

    char *url = strtok(NULL, " ");
    if (url[0] != '/' || url[1] != '/'){
        printf("Wrong URL format.\nCorrect link format: ftp://[<user>:<password>@]<host>/<url-path>");
        return -1;
    }
    url = url+2;

    printf("URL: %s\n", url);

    char *at = strchr(url, '@');
    char *start;

    if (at) {
        char *div = strchr(url, ':');

        if (div) { // username:passoword@url
            strncpy(username, url, div-url);
            strncpy(password, div+1, at-div-1);

            username[div-url] = '\0';
            password[at-div-1] = '\0';
        }
        else { // username@url
            strncpy(username, url, at-url);
            strcpy(password, "annonymous");
        }

        start = at +1;
    }
    else { // url
        strcpy(username, "annonymous");
        strcpy(password, "annonymous");

        start = url;
    }

    char *first_slash = strchr(start, '/');
    char *end = strchr(start, '\0');
    if (first_slash){
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
    decode_rfc1738(argv[1]);
    return 0;
}
