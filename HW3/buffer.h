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
}buffer;

//functions
extern int createBuffer(buffer *thisBuffer, int sizeOfBuffer);
