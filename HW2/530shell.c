//Aaron Zhang
// I certify that no unauthorized assistance has been received or
//given in the completion of this work
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int main(){
    while(1){
        //Print prompt % for user input
        printf("%% ");
        //Take stdin from user, read line until a newline
        int charInputLimit = 5;
        char input[charInputLimit+1];//Accounts for /0 with fgets
        //Check if there is an error with returning the string paremeter
        if(fgets(input,charInputLimit+1,stdin)==NULL){
            if(feof(stdin)!=0){
                //EOF is set, return
                return 0;
            }else{
                fprintf(stderr, "\nError with taking in input\n");
                exit(EXIT_FAILURE);
            }
        }
        //Check if string input contains a newline, if not print to stderr
        if(strchr(input,'\n')==NULL){
            //No newline, meaning that the input was greater than the character limit
            fprintf(stderr, "Input greater than character limit of %i.\n",charInputLimit);
            exit(EXIT_FAILURE);
        }
        //Fork process to child process
        
        //Parse input
        char *filePath;
        char *whitespaceSeperators =" \t\v\f\r";
        filePath = strtok(input,whitespaceSeperators);
        printf("%s",filePath);
        char *token = strtok(NULL,whitespaceSeperators);
        printf("%s",token);
    }
    return 0;
}
