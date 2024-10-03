# Genera el reventador de claves
# Variables
CC = cc
TARGET = reventador
OBJS = generador.o reventador.o probar-clave.o

# Regla por defecto
all: $(TARGET)

# Regla para el ejecutable
$(TARGET): $(OBJS) synced
	rm synced
	$(CC) -o $(TARGET) $(OBJS)

# Regla para generador.o
generador.o: generador.c generador.h synced
	$(CC) -c generador.c

# Regla para reventador.o
reventador.o: reventador.c generador.h synced
	$(CC) -c reventador.c

# Regla para probar-clave.o
probar-clave.o: probar-clave.c generador.h synced
	$(CC) -c probar-clave.c
synced:
	syncwin
	touch synced

# Regla para limpiar los archivos compilados
clean:
	rm -f $(OBJS) $(TARGET)
	rm -f synced

