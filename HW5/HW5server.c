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

int sendToClient(int pid,Socket clientConnection);
char *getCopiedString(char* input);
int getInput(char *destination,Socket clientConnection);
int runShell(Socket clientConnection);
int getWhitespaceSeperatorCount(char *input);
int handleArguments(char *copyInput, char *argArray[], int whitespaceSeperatorCount);
//int runShell(char *input, Socket clientConnection);

int main (int argc, const char * argv[]) {
    PORT_NUMBER = atoi(argv[1]);
    welcomeSocket = ServerSocket_new(PORT_NUMBER);
    if (welcomeSocket < 0)
    {
        fprintf(stderr,"Failed new server socket\n");
        return (-1);
    }
    clientConnection = ServerSocket_accept(welcomeSocket);
    printf("about to run shell");
    runShell(clientConnection);
}


int runShell(Socket clientConnection){
    char line_data[MAX_LINE];
    char c;
    int i;
    //active until EOF signal
    while(1){
        //Fork process to child process
        int currentPID = getpid();
        int childPID;
        childPID = fork();
        if(childPID<0){
            fprintf(stderr, "Error with forking child, %s.\n",strerror(errno));
        }else if(childPID==0){
            //This is the child
            
            //Receive input, this should block until newline
            getInput(line_data,clientConnection);
            //Handle args
            char *copyInput = getCopiedString(line_data);
            int whiteSpaceSeperatorCount = getWhitespaceSeperatorCount(line_data);
            printf("wsp %i",whiteSpaceSeperatorCount);
            char *argArray[whiteSpaceSeperatorCount];
            handleArguments(copyInput, argArray, whiteSpaceSeperatorCount);
            printf("%s",argArray[0]);
            //Create file to redirect stdout to
            char fileName[9];
            snprintf(fileName, 9,"tmp%d", currentPID);
            FILE *fp = freopen(fileName, "w+", stdout);
            //Call execvp to run command
            int error = execvp(argArray[0],argArray);
            //close file
            fclose(fp);
            if(error<0){
                fprintf(stderr, "Error with calling exec, %s.\n",strerror(errno));
                //kill child
                exit(1);
            }
        }else{
            pid_t terminated_pid;
            do{
                terminated_pid = wait(NULL);
            }while(terminated_pid!=childPID);
            printf("\nSending to client\n");
            sendToClient(currentPID,clientConnection);
        }
    }
    return 0;
}


int getWhitespaceSeperatorCount(char *input){
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
    return whitespaceSeperatorCount;
}


int handleArguments(char *copyInput, char *argArray[], int whitespaceSeperatorCount){
    char *whitespaceSeperators =" \t\v\f\r\n";
    //how many arguments there really are, this does not encompass the file path
    int loopCount = whitespaceSeperatorCount-1;
    argArray[whitespaceSeperatorCount-1]=NULL; //Array of pointers must be terminated by null pointer
    //get the file path
    char *filePath= strtok(copyInput,whitespaceSeperators);
    argArray[0] = filePath;
    for(int i=1;i<loopCount;i++){
        argArray[i] = strtok(NULL,whitespaceSeperators);
    }
    return 0;
}


int getInput(char *destination,Socket clientConnection){
    printf("receiving user input\n");
    int i;
    char c;
    for (i = 0; i < MAX_LINE; i++)
    {
        c = Socket_getc(clientConnection);
        if (c == EOF)
        {
            fprintf(stderr,"\nSocket_getc EOF or error\n");
            return 0;
        }else
        {
            if (c == '\n')
            {
                destination[i] = c;
                //memset(line_data, 0, MAX_LINE);
                break;
            }
            printf("%c",c);
            destination[i] = c;
        }
    }
    return 1;
}

int sendToClient(int pid,Socket clientConnection){
    char fileName[9];
    snprintf(fileName, 9,"tmp%d", pid);
    FILE *fp = fopen(fileName, "r");
    char ch;
    printf("sending to client");
    while((ch = getc(fp)) != EOF)
    {
        Socket_putc(ch,clientConnection);
    }
    Socket_putc(EOF,clientConnection);
    fclose(fp);
    return 0;
}
char * getCopiedString(char* input){
    char * copyInput = malloc(strlen(input)+1);
    strcpy(copyInput,input);
    return copyInput;
}
