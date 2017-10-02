//
//  Created by Aaron Zhang on 10/02/17.
// I certify that no unauthorized assistance has been received or
//given in the completion of this work
//

#include "buffer.h"

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
    //Need semaphore information
    return 0;
}
bool isEmpty(buffer *thisBuffer){
    if(thisBuffer->isEmpty==1){
        return true;
    }
    return false;
}
