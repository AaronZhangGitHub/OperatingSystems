//
//  Created by Aaron Zhang on 10/10/17.
// I certify that no unauthorized assistance has been received or
//given in the completion of this work
//

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>


#define OUTPUT_SIZE 80
#define READ_INDEX 0
#define WRITE_INDEX 1

//define process functions
int processInput(pid_t childPID);
int processNewlineToSpace(pid_t childPID);
int processDoubleAsterixToCaret(pid_t childPID);
int processOutput(void);

//Create pipe arguments
int inputPipe[2];
int transferPipe[2];
int outputPipe[2];



int main (int argc, const char * argv[]) {
    //process ID variables
    pid_t processInputPID;
    pid_t processNewlineToSpacePID;
    pid_t processDoubleAsterixToCaretPID;
    pid_t processOutputPID;
    
    //create pipes
    if(pipe(inputPipe)==-1){
        fprintf(stderr, "Issue with creating input pipe...exiting\n");
        exit(EXIT_FAILURE);
    }
    if(pipe(transferPipe)==-1){
        fprintf(stderr, "Issue with creating transfer pipe...exiting\n");
        exit(EXIT_FAILURE);
    }
    if(pipe(outputPipe)==-1){
        fprintf(stderr, "Issue with creating output pipe...exiting\n");
        exit(EXIT_FAILURE);
    }
    //get process id of the current process
    processInputPID = getpid();
    processNewlineToSpacePID = fork();
    if(processNewlineToSpacePID !=0){
        //This is a parent
        processInput(processNewlineToSpacePID);
    }else{
        //this is a child
        processDoubleAsterixToCaretPID = fork();
        if(processDoubleAsterixToCaretPID!=0){
            //This is a parent
            processNewlineToSpace(processDoubleAsterixToCaretPID);
        }
        else{
            //this is a child
            processOutputPID = fork();
            if(processOutputPID!=0){
                //this is a parent
                processDoubleAsterixToCaret(processOutputPID);
            }else{
                //child
                processOutput();
            }
        }
    }
    return 0;
}

int processInput(pid_t childPID){
    //close everything but the inputpipe write
    close(inputPipe[READ_INDEX]);
    close(transferPipe[READ_INDEX]);
    close(transferPipe[WRITE_INDEX]);
    close(outputPipe[READ_INDEX]);
    close(outputPipe[WRITE_INDEX]);
    char c = 0;
    while(1){
        c=getchar();
        //write to input pipe
        if(write(inputPipe[WRITE_INDEX], &c, 1)==-1){
            fprintf(stderr, "Error with writing to input pipe");
            exit(EXIT_FAILURE);
        }
        if(c==EOF){
            break;
        }
    }
    close(inputPipe[WRITE_INDEX]);
    return 0;
}
int processNewlineToSpace(pid_t childPID){
    //close everything but inputpipe read and transfer pipewrite
    close(inputPipe[WRITE_INDEX]);
    close(transferPipe[READ_INDEX]);
    close(outputPipe[READ_INDEX]);
    close(outputPipe[WRITE_INDEX]);
    
    char processingChar=0;
    while(1){
        read(inputPipe[READ_INDEX],&processingChar,1);
        if(processingChar=='\n'){
            //check if the character is a newline, if so replace
            processingChar = ' ';
        }
        write(transferPipe[WRITE_INDEX],&processingChar,1);
        if(processingChar==EOF){
            break;
        }
    }
    close(inputPipe[READ_INDEX]);
    close(transferPipe[WRITE_INDEX]);
    return 0;
}
int processDoubleAsterixToCaret(pid_t childPID){
    //close everything but transferpipe read and outputpipe write
    close(inputPipe[READ_INDEX]);
    close(inputPipe[WRITE_INDEX]);
    close(transferPipe[WRITE_INDEX]);
    close(outputPipe[READ_INDEX]);
    bool lastCharWasAsterisk = false;
    char processingChar = 0;
    char pointerToStar = '*';
    while(1){
        read(transferPipe[READ_INDEX],&processingChar,1);
        if(lastCharWasAsterisk){
            if(lastCharWasAsterisk){
                if(processingChar=='*'){
                    processingChar = '^';
                }else{
                    //write *
                    write(outputPipe[WRITE_INDEX],&pointerToStar,1);
                }
                lastCharWasAsterisk = false;
            }
        }else if(processingChar=='*'){
            lastCharWasAsterisk = true;
            //next loop
            continue;
        }
        write(outputPipe[WRITE_INDEX],&processingChar,1);
        if(processingChar==EOF){
            break;
        }
    }
    close(transferPipe[READ_INDEX]);
    close(outputPipe[WRITE_INDEX]);
    return 0;
}

int processOutput(void){
    //Close everything but output read
    close(inputPipe[READ_INDEX]);
    close(inputPipe[WRITE_INDEX]);
    close(transferPipe[READ_INDEX]);
    close(transferPipe[WRITE_INDEX]);
    close(outputPipe[WRITE_INDEX]);
    
    char returnedChar = 0;
    //create an array to record output and a counter
    int counter = 0;
    int output[80];
    while(1){
        read(outputPipe[READ_INDEX],&returnedChar,1);
        if(returnedChar==EOF){
            break;
        }
        //Add the current character into the output array of characters
        output[counter]=returnedChar;
        counter++;
        if(counter==80){
            //print out all the characters if the output array of characters is full
            for (int i =0; i<80;i++){
                printf("%c",output[i]);
            }
            printf("\n");
            counter = 0;
        }
    }
    close(outputPipe[READ_INDEX]);
    return 0;
}


