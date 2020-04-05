# tp-2020-1c-Pandemiga Delibird
UTN's Operating Systems Proyect

# Instrucciones

## VSCODE:

-Clonar el repo

-Abrir vscode

-Archivo->Abrir área de trabajo

-Seleccionar "vscode-workspace"

-En terminal, escribir "make" en el proyecto a ejecutar (p.e dentro de broker)

-Para ejecutar/debuggear, apretar f5

-Alternativamente, Tareas->Ejecutar Tarea->(La que quieras, con o sin valgrind)

-????

-Profit

## CONSOLA:

-Ir a la carpeta del proyecto (p.e dentro de broker)

-Escribir make

-Escribir ./(nombredelproyecto)

-Alternativamente, escribir ./exec o ./exec_valgrind para realizar ambos pasos de una

#### Para agregar más .c/.h a un proyecto:

-Ir al makefile del proyecto

-Agregar el nombre del archivo en _DEPS, _OBJ, o _SHARED_DEPS, _SHARED_OBJ dependiendo de dónde está el archivo a agregar.

-Agregar el include en el .h que lo requiera

-Volver a compilar usando make

