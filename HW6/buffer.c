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
#include <fcntl.h>
#include "buffer.h"

#define SEMAPHORE_NAME "/charSEM"

//Takes in a pointer to a buffer and the size of the buffer, creates the buffer
int createBuffer(buffer *thisBuffer){
    //allocate space for ever character, size of char should be 2
    //thisBuffer->charValues = (char*) malloc((sizeOfBuffer+1) * sizeof(char));
    
    //Set head and tail pointer to 0
    thisBuffer->headPointer = 0;
    thisBuffer->tailPointer = 0;
    //Set current size to be 0
    thisBuffer->currentSize = 0;
    //Set is empty flag to be 1
    thisBuffer->isEmpty = 1;
    //Set max size
    thisBuffer->maxSize = sizeOfBuffer;
    //semaphore implementation, empty number of buffers begin at the size of the buffer
    //thisBuffer->emptyBuffers = sem_open(freeName,O_CREAT,S_IREAD|S_IWRITE,80);
    //thisBuffer->fullBuffers = sem_open(freeName,O_CREAT,S_IREAD|S_IWRITE,0);
    return 0;
}


//buffer is essentially a queue
int deposit(buffer *thisBuffer, char charToAdd){
    //call down on emptyBuffers
    sem_wait(thisBuffer->emptyBuffers);
    thisBuffer->charValues[thisBuffer->tailPointer]=charToAdd;
    //increment tail, account for overlap
    thisBuffer->tailPointer= ((thisBuffer->tailPointer+1)%(thisBuffer->maxSize));
    //Set buffer as not empty
    thisBuffer->isEmpty = 0;
    //increment size
    thisBuffer->currentSize+=1;
    //Call up on fullBuffers
    sem_post(thisBuffer->fullBuffers);
    return 1;
}
char remoove(buffer *thisBuffer){
    sem_wait(thisBuffer->fullBuffers);
    char returnChar = thisBuffer->charValues[thisBuffer->headPointer];
    //increment the head
    thisBuffer->headPointer = (thisBuffer->headPointer+1)%(thisBuffer->maxSize);
    //decrement size
    thisBuffer->currentSize--;
    sem_post(thisBuffer->emptyBuffers);
    return returnChar;
}
