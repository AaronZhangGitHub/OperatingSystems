//aaron zhang
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "Socket.h"
#include <stdlib.h>


#define MAX_LINE 1024

ServerSocket welcomeSocket;
Socket clientConnection;
int PORT_NUMBER;

int sendToClient(FILE *fp,Socket clientConnection);
char * getCopiedString(char* input);
int runShell(char *input, Socket clientConnection);

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
                    //sendToClient(line_data, clientConnection);
                    runShell(line_data, clientConnection);
                    memset(line_data, 0, MAX_LINE);
                    break;
                }
                printf("%c",c);
                line_data[i] = c;
            }
        }
    }
}
char * getCopiedString(char* input){
    char * copyInput = malloc(strlen(input)+1);
    strcpy(copyInput,input);
    return copyInput;
}

int runShell(char *input, Socket clientConnection){
    printf("Running Shell");
    //make a copy of the input string
    char*copyInput = getCopiedString(input);
    
    //Arguments
    char *whitespaceSeperators =" \t\v\f\r\n";
    int whitespaceSeperatorCount = 1;
    char *token = strtok(input,whitespaceSeperators);
    if(token==NULL){
        whitespaceSeperatorCount = 0;
    }
    while(token!=NULL){
        token = strtok(NULL,whitespaceSeperators);
        whitespaceSeperatorCount++;
    }
    if(whitespaceSeperatorCount<2){
        fprintf(stderr, "Proper input not given.\n");
    }
    //how many arguments there really are, this does not encompass the file path
    int loopCount = whitespaceSeperatorCount-1;
    //array for arguments
    char *argArray[whitespaceSeperatorCount];
    argArray[whitespaceSeperatorCount-1]=NULL; //Array of pointers must be terminated by null pointer
    //get the file path
    char *filePath= strtok(copyInput,whitespaceSeperators);
    argArray[0] = filePath;
    for(int i=1;i<loopCount;i++){
        argArray[i] = strtok(NULL,whitespaceSeperators);
    }
    
    //concat filename
    int currentPID = getpid();
    char fileName[9];
    snprintf(fileName, 9,"tmp%d", currentPID);
    printf("%s",fileName);
    FILE *fp;
    //Fork process to child process
    int childPID;
    childPID = fork();
    if(childPID<0){
        fprintf(stderr, "Error with forking child, %s.\n",strerror(errno));
    }else if(childPID==0){
        printf("Calling freopen");
        fp = freopen(fileName, "w+", stdout);
        printf("b4 execvp");
        int error = execvp(filePath,argArray);
        fprintf(stderr,"after execvp");
        if(error<0){
            fprintf(stderr, "Error with calling exec, %s.\n",strerror(errno));
            //kill child
            exit(1);
        }
        fprintf(stderr,"About to send to client");
        sendToClient(fp, clientConnection);
        fclose(fp);
    }else{
        pid_t terminated_pid;
        do{
            terminated_pid = wait(NULL);
        }while(terminated_pid!=childPID);
        printf("Child terminated");
    }
    //deallocate memory
    free(copyInput);
    return 0;
}
int sendToClient(FILE *fp,Socket clientConnection){
    char ch;
    printf("sending to client");
    while((ch = getc(fp)) != EOF)
    {
        Socket_putc(ch,clientConnection);
    }
    Socket_putc(EOF,clientConnection);
    return 0;
}
