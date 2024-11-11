# Funcionamiento de la llamada
- La función ```sys_mprotect(void)``` y ```sys_munprotect(void)```, son llamadas al sistema que protegen dada una dirección de memoria, protegerlas de la escritura o desprotegerlas.

```c
uint64
sys_mprotect(void){
  int len;
  int addr;

  argint(0, &addr);
  argint(1, &len);
  
  return mprotect( (void*)(__INTPTR_TYPE__)addr, len );
}

uint64
sys_munprotect(void){
  int len;
  int addr;

  argint(0, &addr);
  argint(1, &len);
  
  return munprotect( (void*)(__INTPTR_TYPE__)addr, len );
}
```

- Para ambas llamadas de sistema, se retorna las funciones ```mprotect()``` y ```munprotect```. Donde dada una dirección de memoria se aplica una operación lógica al bit ```(1L)>>2```.

# Explicación de los cambios realizados
- Para crear estás funciones que son llamadas por las llamadas a sistemas correspondientes, se tuvo que editar los siguientes archivos:

## Archivo syscall.h
- En el archivo ```syscall.h``` se tuvo que agregar la siguiente línea de código.
```h
#define SYS_mprotect 22
#define SYS_munprotect 23
```
- Esto se hace para agregar una nueva llamda al sistema.

## Archivo syscall.c
- En el archivo ```syscall.c``` se tuvo que agregar las siguientes líneas de código en sus respectivos lugares.
```c
// Prototypes for the functions that handle system calls.
...
extern uint64 sys_mprotect(void);
extern uint64 sys_mprotect(void);
...
static uint64 (*syscalls[])(void) = {
    ...
    [SYS_mprotect] sys_mprotect(void);
    [SYS_munprotect] sys_munprotect(void);
}
``` 
- Esto se hace para editar la matriz de punteros de funciones que utiliza los números definidos anteriormente (índices) como punteros a las llamadas al sistema que se definen en una ubicación diferente.


## Archivo sysproc.c
- En el archivo```sysproc.c``` se agrego la siguiente línea de código.
```c
...
uint64
sys_mprotect(void){
  int len;
  int addr;

  argint(0, &addr);
  argint(1, &len);
  
  return mprotect( (void*)(__INTPTR_TYPE__)addr, len );
}

uint64
sys_munprotect(void){
  int len;
  int addr;

  argint(0, &addr);
  argint(1, &len);
  
  return munprotect( (void*)(__INTPTR_TYPE__)addr, len );
}

```
- El códgio anterior, implementa y define la función llamada de sistema.

## Archivo user.h
- En el archivo ```user.h``` se tuvo que agregar la siguiente línea de código.
```c
//system calls
...
int mprotect(void*, int);
int munprotect(void*, int);
```
- Esta sería la función a la que llama el programa de usuario. Esta función se asignará a la llamada del sistema con el número 22 y 23 que se define como directiva del preprocesador ```SYS_mprotect``` y ```SYS_munprotect``` .

## Archivo usys.pl
- En el archivo ```usys.pl``` se agrego la siguiente línea de código.
```pl
entry("mprotect");
entry("munprotect");
```
- La línea anterior ayuda a que ```mprotect``` y ```munprotect``` se construya en la interfaz

## Archivo defs.h
- En el archiv ```defs.h``` se hace referencia a las funciones ```mprotect``` y ```munprotect``` que son usadas por las llamadas al sistemas de ```sys_mprotect``` y ```sys_munprotect```
```h
// mprotect and munprotect
int             mprotect(void *addr, int len);
int             munprotect(void *addr, int len);
```

## Archivo proc.c
- En el archivo ```proc.c``` se definen las funciones ```mprotect()``` y ```munprotect()```
```c
int
mprotect(void *addr, int len){
  if ((uint64)addr % PGSIZE != 0 || len <= 0) {
    return -1; // Invalid parameters
  }

  struct proc *p = myproc();

  for (int i = 0; i < len; i++) {
      
    pte_t *pte = walk(p->pagetable, (uint64)addr + i * PGSIZE, 0);

    if (*pte == 0 || (*pte & ~PTE_V) == 0) {
      return -1; // Page not have something
    }
    *pte &= ~PTE_W; // Clear writable bit
  }
  return 0;
}
```
- En la función ```mprotect()```, primero se revisa que la dirección ingresada como su largo sean del formato correcto. Luego se crea un puntero que apunte al proceso actual. Después se crea un ciclo para recorrer todas las páginas asociadas a la logitud dada. Dentro de este ciclo, se recoge el Page Entry asociada a la dirección de memoria entregada, para comprobar que no este vacío o sea inválida. Una vez comprobado lo anterior, se obtiene el Page Entry dada una dirección y pagetable usando walk, para obtener el puntero de la dirección física de la dirección virtual,luego se comprueba que existe y es válido, para terminar con la operación lógica AND usando el bit de escritura definido en el opuesto booleano de PTE_W.
- En la función ```munprotect()```, es exactamente igual a ```mprotect()```, exceptuando que en vez de la línea 
```c
*pte &= ~PTE_W;
```
- Se tiene lo siguiente
```c
*pte |= PTE_W;
```
## Archivo protectTest.c
- En el archivo ```protectTest.c``` se creo el siguiente código para probar la nueva llamada del sistema.
```c
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
```

## Archivo Makefile
- En el archivo ```Makefile``` se tuvo que agregar la siguiente línea de código.
```Makefile
UPROGS =\
    ...
    $U/_protectTest\
```
- El código anterior, sirve para que a la hora de compilar el kernel, el archivo ```protectTest.c``` también se incluya en la compilación.

# Dificultades y soluciones
- En ```sysproc.c```, existieron 3 problemas. 
1. El primero que sería la definición de las funciones ```mprotect()``` y ```munprotect()``` en un archivo aparte que no fuera ```sysproc.c``` y que la llamadas del sistemas fueran ```sys_mprotect(void)``` y no ```sys_mprotect(addr, len)```. 
2. El segundo problema fue tener variables con largos distintos para la variable ```(void *)addr```.
3. El tercer problema fue definir correctamente en ```sys_mprotect(void)``` las variables que se usarian en ```mprotect()```.
- Los problemas anteriores se resolvieron, en primer lugar, 
1. Definiendo para este caso la funcion ```mprotect()``` en ```defs.h``` y escribir la función en ```proc.h```
2. Usar ```(void *)(__INTPTR_TYPE__)addr```, para igualar los largos del ```(void *)``` con ```int```
3. Se siguio el tuvo que agregar ```argint()``` para cada variable usada.
