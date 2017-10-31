//aaron zhang
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "Socket.h"

#define MAX_LINE 1024

ServerSocket welcomeSocket;
Socket clientConnection;
int PORT_NUMBER;

int sendToClient(char *line_data,Socket clientConnection);

int main (int argc, const char * argv[]) {
    PORT_NUMBER = atoi(argv[1]);
    welcomeSocket = ServerSocket_new(PORT_NUMBER);
    char line_data[MAX_LINE];
    if (welcomeSocket < 0)
    {
        printf("Failed new server socket\n");
        return (-1);
    }
    //server runs indefinitely
    char c;
    int i;
    clientConnection = ServerSocket_accept(welcomeSocket);
    while(1){
        for (i = 0; i < MAX_LINE; i++)
        {
            c = Socket_getc(clientConnection);
            if (c == EOF)
            {
                printf("\nSocket_getc EOF or error\n");
                return 0; /* assume socket EOF ends service for this client */
            }
            else
            {
                if (c == '\n')
                {
                    line_data[i] = c;
                    sendToClient(line_data, clientConnection);
                    memset(line_data, 0, MAX_LINE);
                    break;
                }
                printf("%c",c);
                line_data[i] = c;
            }
        }
    }
}

int sendToClient(char *line_data,Socket clientConnection){
    int count = strlen(line_data) + 1; /* include '\0' in count */
    printf("%i",count);
    char c;
    char rc;
    int i;
    for (i = 0; i < count; i++)
    {
        c = line_data[i];
        rc = Socket_putc(c, clientConnection);
        if(c == '\n'){
            return 0;
        }
        if (rc == EOF)
        {
            Socket_putc(EOF, clientConnection);
            printf("Socket_putc EOF or error\n");
            return 0;  /* assume socket EOF ends service for this client */
        }
    }
}
