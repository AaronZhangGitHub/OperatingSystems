//
//  Created by Aaron Zhang on 10/20/17.
// I certify that no unauthorized assistance has been received or
//given in the completion of this work
//
/*
 * 
 * This is my buffer.h program. This is to be used as a shared memory map.
 * the rest is pretty self explanatory
 */
#include "semaphore.h"

typedef struct{
    char charValues[80];
    //Head and tail of the array
    int headPointer;
    int tailPointer;
    int currentSize;
    int isEmpty;
    int maxSize;
    sem_t *fullBuffers;
    sem_t *emptyBuffers;
}buffer;

//functions
extern int createBuffer(buffer *thisBuffer);
extern int deposit(buffer *thisBuffer, char charToAdd);
extern char remoove(buffer *thisBuffer);
