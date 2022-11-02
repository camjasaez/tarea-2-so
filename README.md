# Tarea 2: Hebras - Sistemas Operativos

_**Camilo Saez Garrido, IECI seccion 1**_

## Introducción

La tarea consiste en la creacion de un programa que sea capaz de monitorizar un grupo
de dispositivos de red mediante pruebas
de conectividad y determinar el estado en que se encuentran cada uno de ellos.

## Correr el proyecto

Para correr el proyecto se debe ejecutar el siguiente comando en la carpeta raiz del
proyecto

Primero se compila: 
```bash
make all
```

Luego se ejecuta:
```bash
make run npaq=<numero de paquetes>
```

### Ejemplo

`make run npaq=10 ` : _Corre el programa con 10 paquetes_

npaq es una abreviacion a numero de paquetes que se van a enviar. 