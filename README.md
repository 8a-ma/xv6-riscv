# Funcionamiento de la llamada
- La función ```sys_chmod(void)```, es una llamda al sistema que cambia el modo de un archivo entre lectura, escritura o todos.
  
```c
uint64
sys_chmod(void)
{
  char path[MAXPATH];
  int mode;
  
  if(argstr(0, path, MAXPATH) < 0) return -1;
  argint(1, &mode);

  struct inode *ip = namei(path);

  ilock(ip);
  ip->mode=mode;
  iunlock(ip);

  return 1;
}
```

- Para esta llamada de sistema, se necesita de la dirección donde está alojado el archivo y del modo al cuál se quiere cambiar.

# Explicación de los cambios realizados
- Para crear la función ```chmod()``` como llamada al sistema, se tuvo que editar los siguientes archivos:
  - [syscall.h](#archivo-syscallh)
  - [syscall.c](#archivo-syscallc)
  - [sysfile.c](#arhcivo-sysfilec)
  - [file.h](#archivo-fileh)
  - [fs.h](#arhivo-fsh)
  - [fs.c](#archivo-fsc)
  - [stat.h](#archivo-stath)
  - [mkfs.c](#archivo-mkfsc)
  - [ls.c](#archivo-lsc)
  - [usys.pl](#archivo-usyspl)
  - [user.h](#archivo-userh)

## Archivo syscall.h
- En el archivo ```syscall.h``` se tuvo que agregar la siguiente línea de código.
```h
#define SYS_chmod 22
```
- Esto se hace para agregar una nueva llamda al sistema.

## Archivo syscall.c
 En el archivo ```syscall.c``` se tuvo que agregar las siguientes líneas de código en sus respectivos lugares.
```c
// Prototypes for the functions that handle system calls.
...
extern uint64 syschmod(void);
...
static uint64 (*syscalls[])(void) = {
    ...
    [SYS_chmod] syschmod(void);
}
``` 
- Esto se hace para editar la matriz de punteros de funciones que utiliza los números definidos anteriormente (índices) como punteros a las llamadas al sistema que se definen en una ubicación diferente.

## Arhcivo sysfile.c
- En el archivo ```sysfile.c``` se agrego las siguientes líneas de código.
```c
...
int
validRead(char *path){
  char s[20];
  struct inode *iParent = nameiparent(path, s);

  ilock(iParent);
  int modeParent = iParent->mode;
  iunlock(iParent);

  if(modeParent & M_READ)
    return 1;

  return -1;
}

...

int
validWrite(char *path){
  char s[20];
  struct inode *iParent = nameiparent(path, s);

  ilock(iParent);
  int modeParent = iParent->mode;
  iunlock(iParent);

  if(modeParent & M_WRITE){
    return 1;
  }

  return -1;

}

...

uint64
sys_open(void)
{
    ...

  // Reglas de open read only
    if(omode&O_RDONLY || omode&O_RDWR){
      if(validRead(path) <0 )
        return -1;
    }

    // Reglas open write only
    if(omode&O_WRONLY || omode&O_RDWR || omode&O_CREATE){
      if(validWrite(path) <0)
        return -1;
    
    }
    ...
}


...

uint64
sys_chmod(void)
{
  char path[MAXPATH];
  int mode;
  
  if(argstr(0, path, MAXPATH) < 0) return -1;
  argint(1, &mode);

  struct inode *ip = namei(path);

  ilock(ip);
  ip->mode=mode;
  iunlock(ip);

  return 1;
}
```
- El códgio anterior, implementa y define la función llamada de sistema, además de validar que los permisos de lectura o escritura sean los correctos.

## Archivo file.h
- En el archivo ```file.h``` se agrego las siguientes líneas de código.
```h
struct file {
    ...
    uint mode; // read, write, both
}

...

struct inode {
    ...
    uint mode; // read, write, both
}
```
- Estas modificaciones permiten crear el campo ```mode``` dentro de ambas estructuras.

## Arhivo fs.h
- En el archivo ```fs.h``` se agrego las siguientes líneas de código
```h
struct dinode {
    ...
    uint mode;
}
```
- Estas modificaciones permiten crear el campo ```mode``` dentro de ambas estructuras.

## Arhivo fs.c
- En el archivo ```fs.c``` se agrego las siguientes líneas de código.
```c
struct inode*
ialloc(uint dev, short type)
{
    ...
    for(inum = 1; inum < sb.ninodes; inum++){
        ...
        dip->mode = M_ALL;
        ...
    }
    ...
}

...

void
iupdate(struct inode *ip)
{
    ...
    dip->mode = ip->mode;
    ...
}

...

void
ilock(struct inode *ip)
{   
    ...
    ip->mode = dip->mode;
    ...
}

...

void
stati(struct inode *ip, struct stat *st)
{
    ...
    st->mode = ip->mode;
    ...
}

```
- Esta modificaciones permiten el intercambio de ```mode``` entre las diferentes estructuras y nodos.

## Archivo stat.h
- En el archivo ```stat.h``` se agrego las siguientes líneas de código.
```h
#define M_READ    1   // Read mode
#define M_WRITE   2   // Write Mode
#define M_ALL     3   // All mode 

struct stat {
    ...
    uint mode;
}
```
- Esta modificaciones permiten definir los distintos modos posibles en variables, además de agregar el campo ```mode``` dentro de la estructura

## Archivo mkfs.c
- En el archivo ```mkfs.c``` se agrego las siguientes líneas de código.
```c
...

// assert((BSIZE % sizeof(struct dinode)) == 0);

...

uint
ialloc(ushort type)
{
    ...
    din.mode = M_ALL;
    ...
}
```
- Estas modificaciones permiten establecer como mode lectura y escritura a los archivos que se inicializen usando ```ialloc```.

## Archivo ls.c
- En el archivo ```ls.c``` se agrego las siguientes líneas de código.
```c
...

void
ls(char *path)
{   
    switch(st.type){
    ...
    case T_DIR:
        ...
        printf("%s %d %d %d %d\n", fmtname(buf), st.type, st.ino, (int) st.size, st.mode);
    }
    ...
}

...
```
- Esta modificación permite visualizar el modo de un archivo cuando se usa el comando ```ls```.

## Archivo usys.pl
- En el archivo ```sysfile.c``` se agrego las siguientes líneas de código.
```pl
entry("chmod");
```
- Esta modificación ayuda a que ```chmod``` se construya dentro de la interfaz

## Archivo user.h
- En el archivo ```sysfile.c``` se agrego las siguientes líneas de código.
```h
int chmod(const char*, int);
```
- Esta sería la función a la que llama el programa de usuario. Esta función se asignará a la llamada del sistema con el número 22 que se define como directiva del preprocesador ```SYS_chmod```.

# Dificultades y soluciones
- A la hora de compilar, existia un problema con ```assert((BSIZE % sizeof(struct dinode)) == 0);```, debido a que ```dinode``` superaba la cantidad de bits necesarios para que el cuociente con ```BSIZE``` sea igual a 0, por lo que, se dejo comentado.
- Existio un porblema a la hora de inicializar ```path``` dentro de ```sys_chmod()```, por lo que, se reviso a ```sys_mkdir()``` para entender que modificaciones era necesaria.

# Referencias bibliográficas
- [xv6 labs @KUL](https://os.edu.distrinet-research.be/labs/file-systems/3_access_perm/) 
- [xv6 chmod and usergroup](https://github.com/gojkovicmatija99/Users-and-groups-in-xv6/blob/master/)
- 