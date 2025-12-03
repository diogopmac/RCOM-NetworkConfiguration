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

        // username:passoword@url
        if (div){
            strncpy(username, url, div-url);
            strncpy(password, div+1, at-div-1);

            username[div-url] = '\0';
            password[at-div-1] = '\0';
        }
        // username@url
        else {
            strncpy(username, url, at-url);
            strcpy(password, "annonymous");
        }

        start = at +1;
    }
    // url
    else {
        strcpy(username, "annonymous");
        strcpy(password, "annonymous");

        start = url;
    }

    printf("%s\n", start);

    printf("Username: %s\n", username);
    printf("Password: %s\n", password);
    


    return 0;
}

int main(int argc, char **argv){
    decode_rfc1738(argv[1]);
    return 0;
}
