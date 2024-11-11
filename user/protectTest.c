#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

int 
main(int argc, char *argv[]){
    char *addr = sbrk(0);
    sbrk(4096);
    
    if (mprotect(addr, 1) == -1){
        printf("Mprotect falló\n");
        exit(1);
    }

    char *ptr = addr;
    *ptr = 'A';
    printf("======Mprotect======\n");
    printf("Valor en la dirección: %s\n", ptr);
    printf("Valor en la dirección: %s\n", addr);
    

     if (munprotect(addr, 1) == -1){
        printf("Mprotect falló\n");
        exit(1);
    }
    printf("======Munprotect======\n");
    printf("Valor en la dirección: %s\n", ptr);
    printf("Valor en la dirección: %s\n", addr);

    exit(1);
}