//  Created by Aaron Zhang
// I certify that no unauthorized assistance has been received or
//given in the completion of this work
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "Socket.h"
#include <stdlib.h>
/*
 * main creates the welcome socket and then initiates run shell
 * run shell performs the exec and fork functions, calls getInput, getCopiedString, getWhitespaceSeperatorCount
 * ,handleArguments, and sendErrorToUser.
 * getInput blocks until a newline is received from the user, this is the users input
 * getWhitespaceSeperatorCount gets the count of whitespace seperators between the input
 * handleArguments creates an array of strings from the user input which is used to call execvp
 * sendErrorToUser sends any error messages to the client via the port connection
 */

#define MAX_LINE 1024

ServerSocket welcomeSocket;
Socket clientConnection;
int PORT_NUMBER;

int sendToClient(int pid,Socket clientConnection);
char *getCopiedString(char* input, Socket clientConnection);
int getInput(char *destination,Socket clientConnection);
int runShell(Socket clientConnection);
int getWhitespaceSeperatorCount(char *input,Socket clientConnection);
int handleArguments(char *copyInput, char *argArray[], int whitespaceSeperatorCount);
int sendErrorToUser(char *errorDescription, Socket clientConnection);

int main (int argc, const char * argv[]) {
    PORT_NUMBER = atoi(argv[1]);
    welcomeSocket = ServerSocket_new(PORT_NUMBER);
    if (welcomeSocket < 0)
    {
        char *error = "Failed new server socket\n";
        fprintf(stderr,"%s",error);
        return (-1);
    }
    clientConnection = ServerSocket_accept(welcomeSocket);
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
            char *error = "Error with forking child, %s.\n";
            fprintf(stderr,"%s",error);
            sendErrorToUser(error,clientConnection);
        }else if(childPID==0){
            //This is the child
            
            //Receive input, this should block until newline
            getInput(line_data,clientConnection);
            //Handle args
            char *copyInput = getCopiedString(line_data, clientConnection);
            int whiteSpaceSeperatorCount = getWhitespaceSeperatorCount(line_data,clientConnection);
            char *argArray[whiteSpaceSeperatorCount];
            handleArguments(copyInput, argArray, whiteSpaceSeperatorCount);
            //Create file to redirect stdout to
            char fileName[9];
            snprintf(fileName, 9,"tmp%d", currentPID);
            FILE *fp = freopen(fileName, "w+", stdout);
            //Call execvp to run command
            int error = execvp(argArray[0],argArray);
            //close file
            fclose(fp);
            free(copyInput);
            if(error<0){
                char *error = "Error with calling exec, %s.\n";
                fprintf(stderr,"%s",error);
                sendErrorToUser(error,clientConnection);
                //kill child
                exit(1);
            }
        }else{
            pid_t terminated_pid;
            do{
                terminated_pid = wait(NULL);
            }while(terminated_pid!=childPID);
            sendToClient(currentPID,clientConnection);
        }
    }
    return 0;
}


int getWhitespaceSeperatorCount(char *input, Socket clientConnection){
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
        char *error = "Proper input not given.\n";
        fprintf(stderr,"%s",error);
        sendErrorToUser(error, clientConnection);
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
    int i;
    char c;
    for (i = 0; i < MAX_LINE; i++)
    {
        c = Socket_getc(clientConnection);
        if (c == EOF)
        {
            char *error = "\nSocket_getc EOF or error\n";
            fprintf(stderr,"%s",error);
            sendErrorToUser(error, clientConnection);
            return 0;
        }else
        {
            if (c == '\n')
            {
                destination[i] = c;
                break;
            }
            destination[i] = c;
        }
        if(i==MAX_LINE-1){
            //Acount for if the user has exceeded input
            memset(destination, 0, MAX_LINE);
            i = 0;
        }
    }
    return 1;
}

int sendToClient(int pid,Socket clientConnection){
    char fileName[9];
    snprintf(fileName, 9,"tmp%d", pid);
    FILE *fp = fopen(fileName, "r");
    char ch;
    while((ch = getc(fp)) != EOF)
    {
        Socket_putc(ch,clientConnection);
    }
    Socket_putc(EOF,clientConnection);
    fclose(fp);
    remove(fileName);
    
    return 0;
}
char * getCopiedString(char* input, Socket clientConnection){
    char * copyInput = malloc(strlen(input)+1);
    strcpy(copyInput,input);
    return copyInput;
}
int sendErrorToUser(char *errorDescription, Socket clientConnection){
    int i;
    for(i=0;i<strlen(errorDescription);i++){
        Socket_putc(errorDescription[i],clientConnection);
    }
    Socket_putc(EOF,clientConnection);
    return 0;
}
