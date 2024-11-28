#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user.h"

int
main(int argc, char *argv[]){
    int fd = open("test", O_CREATE | O_RDWR);
    if (fd >= 0){
        printf("File create id: %d\n", fd);
    }
    else{
        printf("File create error\n");
        exit(0);
    }

    chmod("test", M_READ);
    close(fd);
    
    exit(1);
}