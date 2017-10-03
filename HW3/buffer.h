//
//  Created by Aaron Zhang on 10/02/17.
// I certify that no unauthorized assistance has been received or
//given in the completion of this work
//
typedef struct{
    char *charValues;
    //Head and tail of the array
    int headPointer;
    int tailPointer;
    int currentSize;
    int isEmpty;
    int maxSize;
    semaphore fullBuffers;
    seamphore emptyBuffers;
}buffer;

//functions
extern int createBuffer(buffer *thisBuffer, int sizeOfBuffer);
extern bool isEmpty(buffer *thisBuffer);
extern bool isFull(buffer *thisBuffer);
extern char peek(buffer *thisBuffer);
extern int deposit(buffer *thisBuffer, char charToAdd);
extern char remoove(buffer *thisBuffer);
