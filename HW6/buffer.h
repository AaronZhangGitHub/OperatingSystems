//
//  Created by Aaron Zhang on 10/02/17.
// I certify that no unauthorized assistance has been received or
//given in the completion of this work
//
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
