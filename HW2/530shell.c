//Aaron Zhang
// I certify that no unauthorized assistance has been received or
//given in the completion of this work
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

int main(){
    while(1){
        //Print prompt % for user input
        printf("%% ");
        //Take stdin from user, read line until a newline
        int charInputLimit = 75;
        char input[charInputLimit+1];//Accounts for /0 with fgets
        //Check if there is an error with returning the string paremeter
        if(fgets(input,charInputLimit+1,stdin)==NULL){
            if(feof(stdin)!=0){
                //EOF is set, return
                return 0;
            }else{
                fprintf(stderr, "\nError with taking in input\n");
            }
        }
        //Check if string input contains a newline, if not print to stderr
        if(strchr(input,'\n')==NULL){
            //No newline, meaning that the input was greater than the character limit
            fprintf(stderr, "Input greater than character limit of %i.\n",charInputLimit);
            //Need to clear stdin
            char c;
            while ((c = getchar()) != '\n' && c != EOF) { }
            continue; //do not consider this as input
        }
        //make a copy of the input string
        char * copyInput = malloc(strlen(input)+1);
        strcpy(copyInput,input);
        
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
            fprintf(stderr, "Proper input not given\n");
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
        //Fork process to child process
        
        int childPID;
        childPID = fork();
        if(childPID<0){
            fprintf(stderr, "Error with forking child.\n");
        }else if(childPID==0){
            int error = execvp(filePath,argArray);
            if(error<0){
                fprintf(stderr, "Error with calling exec.\n");
                //kill child
                exit(1);
            }
        }else{
            pid_t terminated_pid;
            do{
                int waitStatus;
                terminated_pid = wait(NULL);
            }while(terminated_pid!=childPID);
        }
        free(copyInput);
    }
    return 0;
}
