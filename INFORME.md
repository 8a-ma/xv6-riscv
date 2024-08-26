# Instalación
## Requisitos
- Tener instalado Git
- Tener instalado Docker
- Tener instalado make
- Tener alguna distribución linux como S.O. principal
## Instalación
- Se debe de abrir la terminal clonar repositorio del xv6
~~~bash
$ git clone https://github.com/otrab/xv6-riscv.git xv6
~~~
- Entrar en la carpeta recién creada
~~~bash
$ cd ./xv6
~~~
- Descargar imagen de docker
~~~bash
$ docker pull ghcr.io/francisrstokes/rv-toolchain-docker:main
~~~
- Desde la carpeta <code>xv6</code>, entrar en la terminal y ejecutar los siguientes comandos, remplazando el ID_IMAGE por la id de la imagen descargada en el paso anterior
~~~bash
$ sudo docker run --rm -v "$(pwd)":/project -w /project -it <ID_IMAGE> make
$ sudo docker run --rm -v "$(pwd)":/project -w /project -it <ID_IMAGE> make fs.img
~~~
- Ya instalado lo anterior, ejecutar el siguiente comando para iniciar el xv6
~~~bash
$ make qemu
~~~
![Tarea 0](./statics/Tarea%200.png)

# Problemas y soluciones
- El primer problema con que se encontro, fue no tener instalado el toolchain para poder compilar el xv6, por lo que, se tuvo que instalar los toolchain correspondientes, usando una de las tres siguientes opciones
~~~bash
$ sudo apt-get update
$ sudo apt-get install gcc-riscv-linux-gnu
~~~
~~~bash
$ sudo apt-get update
$ sudo apt-get install gcc-riscv64-unknown-elf
~~~
~~~bash
$ cd ~
$ git clone https://github.com/riscv-collab/riscv-gnu-toolchain.git
~~~
- Sin embargo, el siguiente problema fue que en ninguno de los 3 toolchain compilaba correctamente, ya que, en los 3 toolchain, la consola siempre daba el siguiente error
![Imagen](./statics/Problema%201.png)
- Por lo que, para solucionar, ambos problemas, se decidio compilar xv6, usando una imagen del toolchain en docker.
- Al compilar el xv6 usando el toolchain en docker, el programa no ejecutaba debido a que no se lograba encontrar el programa en <code>/opt/riscv/lib/gcc/riscv64-unknown-linux-gnu/12.2.0/include/stdarg.h</code>
- Por lo que, rebisando la ruta de la carpeta, se llego a concluir de que, efectivamente jamás iba a existir el archivo stdarg.h, ya que, la version que se tiene la carpeta <code>/opt/riscv/lib/gcc/riscv64-unknown-linux-gnu/13.2.0</code> y no la <code>/opt/riscv/lib/gcc/riscv64-unknown-linux-gnu/12.2.0</code>, concluyendo de que, para solucionar el problema simplemente se cambio el nombre de la carpeta <code>12.2.0</code> a <code>13.2.0</code>, logrando así ejecutar el xv6 con qemu.