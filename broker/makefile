#Importante: Dejar todas las carpetas en el formato en el que están
# a la hora de compilar, 
#Incluida la shared que está fuera de este proceso 
#Si te saltan errores en vscode, importar [solo] la carpeta de 1 proyecto (broker p.ej)
# y no la de Delibird. Tambien funciona archivo->agregar carpeta al area de trabajo,
#pero siempre agregando uno por uno los proyectos
#Alternativamente abrir el area de trabajo con el archivo vscode-workspace

IDIR =./include
SHARED_IDIR = ../shared/include
CC=gcc
CFLAGS=-I$(IDIR) -I$(SHARED_IDIR) -g -Wall

ODIR=./obj
SRCDIR =./src
LIBS=-lcommons -lpthread

SHARED_SRCDIR = ../shared/src
SHARED_ODIR = ../shared/obj
##### en _DEPS se agregan los .h, por ejemplo "_DEPS = server.h utils.h queues.h"
_DEPS = broker.h initializer.h finalizer.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))
##### en _OBJ se agregan todos los .c pero cambiandolo por .o, 
# ej "_OBJ = server.o utils.o queues.o"
_OBJ = broker.o initializer.o finalizer.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

#lo de abajo dice "para generar ./obj/(loquesea).o, agarrá
# ./src/(loquesea).c y todos los .h de include y de la shared,
# y compilalo"
$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS) $(SHARED_DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
#lo de abajo dice "para generar ../shared/obj/(loquesea).o, agarrá
# ../shared/src/(loquesea).c y todos los .h de la shared,
# y compilalo"
$(SHARED_ODIR)/%.o: $(SHARED_SRCDIR)/%.c $(SHARED_DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
##### en _SHARED_DEPS se agregan los .h de la shared library,
# ej "_SHARED_DEPS = common_utils.h common_otrafuncion.h"
_SHARED_DEPS = common_utils.h exit_status.h
SHARED_DEPS = $(patsubst %,$(SHARED_IDIR)/%,$(_SHARED_DEPS))
##### en _SHARED_OBJ se agregan los .c de la shared library pero con .o
# ej "_SHARED_OBJ = common_utils.o common_otrafuncion.o"
_SHARED_OBJ = common_utils.o
SHARED_OBJ = $(patsubst %,$(SHARED_ODIR)/%,$(_SHARED_OBJ))


#esto dice "para compilar broker, necesito todos los .o de este 
#proyecto y de la shared, y le voy a meter las librerias e includes que necesito"
broker: $(OBJ) $(SHARED_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean
#esto dice "eliminá todo guachin: el ejecutable tambien"
clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ broker $(SHARED_ODIR)/*.o 