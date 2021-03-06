//Aaron Zhang

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "Socket.h"

int PORT_NUMBER;

char * getInput(int charInputLimit);
void sendInput(Socket connect_socket);
void receiveInput(Socket connect_socket);

int main (int argc, const char * argv[]) {
    Socket connect_socket;
    //Check if there are the appropriate number of arguments
    if(argc!=3){
        fprintf(stderr,"Not the correct number of arguments.\n");
        exit(0);
    }
    char dns_host_name[200];
    strcpy(dns_host_name, argv[1]);
    PORT_NUMBER = atoi(argv[2]);
    printf("%s %i\n",dns_host_name,atoi(argv[2]));
    //connect to socket
    
    connect_socket = Socket_new(dns_host_name, PORT_NUMBER);
    if (connect_socket < 0)
    {
        printf("Failed to connect to server\n");
        return (-1);
    }
    sendInput(connect_socket);
    
}

void receiveInput(Socket connect_socket){
    char c;
    while(c!=EOF && c!='\n'){
        c = Socket_getc(connect_socket);
        printf("%c",c);
    }
}

void sendInput(Socket connect_socket){
    char c;
    int count = 0;
    while((c = getchar())!= EOF){
        count++;
        Socket_putc(c,connect_socket);
        if(c=='\n'){
            //wait for input
            receiveInput(connect_socket);
        }
    }
    Socket_putc(3,connect_socket);
}
