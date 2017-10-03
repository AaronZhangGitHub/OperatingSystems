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
#include "semaphore.h"
#include "st.h"

//Takes in a pointer to a buffer and the size of the buffer, creates the buffer
int createBuffer(buffer *thisBuffer, int sizeOfBuffer){
    //allocate space for ever character, size of char should be 2
    thisBuffer->charValues = (char*) malloc((sizeOfBuffer+1) * sizeof(char));
    
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
    createSem(&(thisBuffer->fullBuffers),0);
    createSem(&(thisBuffer->emptyBuffers),sizeOfBuffer);
    return 0;
}
bool isEmpty(buffer *thisBuffer){
    if(thisBuffer->isEmpty==1){
        return true;
    }
    return false;
}
bool isFull(buffer *thisBuffer){
    if(thisBuffer->currentSize==thisBuffer->maxSize){
        return true;
    }
    return false;
}
char peek(buffer *thisBuffer){
    if(isEmpty(thisBuffer)){
        //null val
        return 0;
    }
    return thisBuffer->charValues[thisBuffer->headPointer];
}
//buffer is essentially a queue
int deposit(buffer *thisBuffer, char charToAdd){
    //call down on emptyBuffers
    down(&(thisBuffer->emptyBuffers));
    if(!isFull(thisBuffer)){
        //add character
        thisBuffer->charValues[thisBuffer->head]=charToAdd;
        //increment tail, account for overlap
        thisBuffer->tailPointer= ((thisBuffer->tailPointer+1)%thisBuffer->(maxSize));
        //Set buffer as not empty
        thisBuffer->isEmpty = 0;
        //increment size
        thisBuffer->currentSize+=1;
    }
    up(&(thisBuffer->fullBuffers));
    return 1;
}
char remoove(buffer *thisBuffer){
    down(&(thisBuffer->fullBuffers));
    char returnChar;
    if(isEmpty(thisBuffer)){
        //do nothing
    }else if(thisBuffer->headPointer==thisBuffer->tailPointer){
        //head and tail are equal last value in array
        returnChar = thisBuffer->charValues[thisBuffer->headPointer];
        thisBuffer->isEmpty=1;
    }else{
        returnChar = thisBuffer->charValues[thisBuffer->headPointer];
        thisBuffer->headPointer = (thisBuffer->headPointer+1)%thisBuffer->maxSize;
    }
    //decrement size
    thisBuffer->currentSize--;
    up(&(thisBuffer->emptyBuffers));
    return returnChar;
}
