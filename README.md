# Tarea 2: Hebras - Sistemas Operativos

_**Camilo Saez Garrido, IECI seccion 1**_

## Introducción

La tarea consiste en la creacion de un programa que sea capaz de monitorizar un grupo
de dispositivos de red mediante pruebas
de conectividad y determinar el estado en que se encuentran cada uno de ellos.

## Correr el proyecto

Para correr el proyecto se debe ejecutar el siguiente comando en la carpeta raiz del
proyecto

Se ejecuta:
```bash
make run na=<nombre del fichero> npaq=<numero de paquetes>
```

### Ejemplo

`make run na=archivo_listado_ips.txt npaq=10 ` : _Corre el programa con 10 paquetes_

npaq es una abreviacion a numero de paquetes que se van a enviar. 
na es una abreviacion a nombre de archivo.

## Importante

- Para que el programa funcione correctamente, debe existir la carpeta `logs` en la raiz y ademas de que el archivo con el listado de las ips tambien este en la raiz del proyecto.

- En linux, debe tener instalado g++ , make,  gdb y limbomp-dev

- El fichero con las ip debe ser .txt