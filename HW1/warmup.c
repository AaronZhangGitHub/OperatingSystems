//
//  warmup.c
//  
//
//  Created by Aaron Zhang on 8/30/17.
// I certify that no unauthorized assistance has been received or
//given in the completion of this work
//

#include <stdio.h>
#include <unistd.h>

//Need to add a check if, the last character is an *, see if the next char is an *

int main(){
    int outputLength = 4;
    char c;
    char input[outputLength];
    int i =0;
    
    while ((c = getchar()) != EOF){
        if(c=='\n'){
            //replace with a space
            c = ' ';
        }else if(c=='*'){
            //check if next input is a *, if so replace
            char temp = getchar(); //next char in the buffer
            if(temp=='*'){
                c = '^';
            }else{
                //push the temporary char back into the buffer, current char will stay as *
                ungetc(temp,stdin);
            }
        }
        input[i] = c;
        i++;
        if(i==outputLength){
            //print results
            for (int i =0; i<sizeof(input)/sizeof(char);i++){
                printf("%c",input[i]);
            }
            //reset value of i to 0, will only trigger if output length characters written again
            printf("\n");//newline added to end of each {outputLength} length char output
            i = 0;
        }
    }
}
