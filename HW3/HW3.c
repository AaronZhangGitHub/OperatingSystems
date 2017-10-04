//
//  Created by Aaron Zhang on 10/02/17.
// I certify that no unauthorized assistance has been received or
//given in the completion of this work
//

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "buffer.h"
#include "st.h"

#define BUFFER_SIZE 80

typedef struct
{
    buffer *consumerBuffer; //Pointer To Consumer
    buffer *producerBuffer; //Pointer To Producer
}producerConsumerPair;

//Need to get functional with i/o
void *characterInput(void *state);
void *newLineToSpace(void *state);
void *doubleAsteriskToCarrot(void *state);
void *characterOutput(void *state);

int main (int argc, const char * argv[]) {
    st_init();
    
    //Initialize producer&consumer pairs, allocate space
    producerConsumerPair *characterInputStruct=malloc(sizeof(producerConsumerPair));
    producerConsumerPair *newLineToSpaceStruct=malloc(sizeof(producerConsumerPair));
    producerConsumerPair *doubleAsteriskToCarrotStruct=malloc(sizeof(producerConsumerPair));
    producerConsumerPair *characterOutputStruct=malloc(sizeof(producerConsumerPair));
    
    //create buffers for each producer consumer piar
    buffer inputBuffer, newLineToSpaceBuffer, outputBuffer;
    createBuffer(&inputBuffer,BUFFER_SIZE);
    createBuffer(&newLineToSpaceBuffer,BUFFER_SIZE);
    createBuffer(&outputBuffer,BUFFER_SIZE);
    
    //set struct values for producer consumer pairs
    characterInputStruct->consumerBuffer = NULL;//This is the first producer
    characterInputStruct->producerBuffer = &inputBuffer;
    
    newLineToSpaceStruct->consumerBuffer = &inputBuffer;
    newLineToSpaceStruct->producerBuffer = &newLineToSpaceBuffer;
    
    doubleAsteriskToCarrotStruct->consumerBuffer = &newLineToSpaceBuffer;
    doubleAsteriskToCarrotStruct->producerBuffer = &outputBuffer;
    
    characterOutputStruct->consumerBuffer = &outputBuffer;
    characterOutputStruct->producerBuffer = NULL;
    
    //create threads
    if(st_thread_create(characterInput,characterInputStruct,0,0)==NULL){
        perror("st_thread_create \n");
        exit(1);
    }
    
    if(st_thread_create(newLineToSpace,newLineToSpaceStruct,0,0)==NULL){
        perror("st_thread_create \n");
        exit(1);
    }
    if(st_thread_create(doubleAsteriskToCarrot,doubleAsteriskToCarrotStruct,0,0)==NULL){
        perror("st_thread_create \n");
        exit(1);
    }
    if(st_thread_create(characterOutput,characterOutputStruct,0,0)==NULL){
        perror("st_thread_create \n");
        exit(1);
    }
    
    fflush(stdout);
    st_thread_exit(NULL);
    return 0;
}
void *characterInput(void *state){
    printf("character input\n");
    producerConsumerPair *producer = state;
    char c;
    
    while (1){
        //deposit into buffer until EOF
        c=getchar();
        deposit(producer->producerBuffer,c);
        if(c==EOF){
            break;
        }
    }
    st_thread_exit(NULL);
}
void *newLineToSpace(void *state){
    producerConsumerPair *pcPair = state;
    char processingChar;
    while(1){
        processingChar = remoove(pcPair->consumerBuffer);
        if(processingChar=='\n'){
            processingChar = ' ';
        }
        deposit(pcPair->producerBuffer,processingChar);
        if(processingChar==EOF){
            break;
        }
    }
    st_thread_exit(NULL);
}
void *doubleAsteriskToCarrot(void *state){
    producerConsumerPair *pcPair = state;
    bool lastCharWasAsterisk = false;
    char processingChar;
    while(1){
        processingChar = remoove(pcPair->consumerBuffer);
        if(lastCharWasAsterisk){
            if(processingChar=='*'){
                deposit(pcPair->producerBuffer,'^');
            }else{
                deposit(pcPair->producerBuffer,'*');
            }
            lastCharWasAsterisk=false;
        }else if(processingChar=='*'){
            lastCharWasAsterisk=true;
        }else{
            deposit(pcPair->producerBuffer,processingChar);
        }
        if(processingChar==EOF){
            break;
        }
    }
    
    st_thread_exit(NULL);
}
void *characterOutput(void *state){
    producerConsumerPair *consumer = state;
    char returnedChar;
    int counter = 0;
    while(1){
        returnedChar = remoove(consumer->consumerBuffer);
        if(returnedChar==EOF){
            printf("\n");
            break;
        }
        if(returnedChar==EOF){
            break;
        }
        printf("%c",returnedChar);
        counter++;
        if(counter==80){
            printf('\n');
            counter = 0;
        }
    }
    st_thread_exit(NULL);
}
