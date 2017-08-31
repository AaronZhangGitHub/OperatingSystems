//
//  warmup.c
//  
//
//  Created by Aaron Zhang on 8/30/17.
//
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
        }else if(c=='*' && i>0){
            //check if previous is also an ast, need handle if it is the first char as well
            if(input[i-1]=='*'){
                c = '^';
                i--;
            }else{
                //current character is an *, need to check if the next is an *
                char temp = getchar();
                if(temp=='*'){
                    c = '^';
                }else{
                    //push back into the buffer
                    ungetc(temp,stdin);
                }
            }
        }
        input[i] = c;
        i++;
        if(i==outputLength){
            //print results
            for (int i =0; i<sizeof(input)/sizeof(char);i++){
                printf("%c",input[i]);//followed by a newline
            }
            //reset value of i to 0, will only trigger if output length characters written again
            printf("\n");//newline
            i = 0;
        }
    }
}
