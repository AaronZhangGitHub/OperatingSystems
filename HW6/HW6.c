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
#include <sys/mman.h>
#include "buffer.h"
#include <sys/types.h> // kill
#include <signal.h>    // kill

#define BUFFER_SIZE 80
#define FULL_SEM "/fullSEM"
#define EMPTY_SEM "/emptySEM"
#define ERROR -1
//Need to get functional with i/o
void *characterInput(void *state);
void *newLineToSpace(void *state);
void *doubleAsteriskToCarrot(void *state);
void *characterOutput(void *state);
void producer(buffer*mappedFile);
void consumer(buffer*mappedFile);
void waitForChildren(pid_t*childpids);
buffer* createMMAP(size_t size);
pid_t forkChild(void (*function)(buffer *), buffer* state);
int main (int argc, const char * argv[]) {
    buffer *buf = createMMAP(sizeof(buffer));
    createBuffer(buf);
    //fork children
    pid_t childpids[2];
    childpids[0] = forkChild(producer,buf);
    childpids[1] = forkChild(consumer,buf);
    
    //wait
    waitForChildren(childpids);
    
    if(sem_unlink(FULL_SEM)==ERROR){
        exit(EXIT_FAILURE);
    }
    if(sem_unlink(EMPTY_SEM)==ERROR){
        exit(EXIT_FAILURE);
    }
}
void waitForChildren(pid_t* childpids){
    int status;
    while(ERROR<wait(&status)){
        if(!WIFEXITED(status)){
            kill(childpids[0],SIGKILL);
            kill(childpids[1],SIGKILL);
            break;
        }
    }
}
void producer(buffer*mappedFile){
    mappedFile->emptyBuffers = sem_open(EMPTY_SEM,O_CREAT,S_IREAD|S_IWRITE,80-mappedFile->currentSize);
    mappedFile->fullBuffers = sem_open(FULL_SEM,O_CREAT,S_IREAD|S_IWRITE,mappedFile->currentSize);
    if(mappedFile->emptyBuffers==SEM_FAILED||mappedFile->fullBuffers==SEM_FAILED){
        exit(EXIT_FAILURE);
    }
    char c;
    bool lastCharWasAsterisk = false;
    while(1){
        c=getchar();
        if(c=='\n'){
            c = ' ';
        }
        if(lastCharWasAsterisk){
            if(c =='*'){
                deposit(mappedFile,'^');
            }else{
                deposit(mappedFile,'*');
            }
            lastCharWasAsterisk = false;
        }else if(c=='*'){
            lastCharWasAsterisk = true;
        }else{
         deposit(mappedFile,c);
        }
        if(c==EOF){
            deposit(mappedFile,c);
            break;
        }
    }
    sem_close(mappedFile->emptyBuffers);
    sem_close(mappedFile->fullBuffers);
}
void consumer(buffer*mappedFile){
    mappedFile->emptyBuffers = sem_open(EMPTY_SEM,O_CREAT,S_IREAD|S_IWRITE,80-mappedFile->currentSize);
    mappedFile->fullBuffers = sem_open(FULL_SEM,O_CREAT,S_IREAD|S_IWRITE,mappedFile->currentSize);
    char c;
    while(1){
        c = remoove(mappedFile);
        printf("%c",c);
        fflush(stdout);
        if(c==EOF){
            break;
        }
    }
    sem_close(mappedFile->emptyBuffers);
    sem_close(mappedFile->fullBuffers);
}
pid_t forkChild(void (*function)(buffer *), buffer* state){
    //This function takes a pointer to a function as an argument
    //and the functions argument. It then returns the forked child's pid.
    
    pid_t childpid;
    switch (childpid = fork()) {
        case ERROR:
            perror("fork error");
            exit(EXIT_FAILURE);
        case 0:
            (*function)(state);
        default:
            return childpid;
    }
}

buffer* createMMAP(size_t size){
    //These are the neccessary arguments for mmap. See man mmap.
    void* addr = 0;
    int protections = PROT_READ|PROT_WRITE; //can read and write
    int flags = MAP_SHARED|MAP_ANONYMOUS; //shared b/w procs & not mapped to a file
    int fd = -1; //We could make it map to a file as well but here it is not needed.
    off_t offset = 0;
    
    //Create memory map
    buffer* state =  mmap(addr, size, protections, flags, fd, offset);
    
    if (( void *) ERROR == state){//on an error mmap returns void* -1.
        perror("error with mmap");
        exit(EXIT_FAILURE);
    }
    return state;
}

void deleteMMAP(void* addr){
    //This deletes the memory map at given address. see man mmap
    if (ERROR == munmap(addr, sizeof(buffer))){
        perror("error deleting mmap");
        exit(EXIT_FAILURE);
    }
}
