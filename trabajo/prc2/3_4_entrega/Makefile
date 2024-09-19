
TARGET=4entrega-bt0354
OBJ_DIR = out
OBJS = $(OBJ_DIR)/main.o $(OBJ_DIR)/procesos.o

$(TARGET): $(OBJS)
	@echo "Linking target..."
	cc $(OBJS) -o $(TARGET)

$(OBJ_DIR)/main.o: main.c  $(OBJ_DIR)
	@echo "Compiling main.c..."
	cc -c main.c -o $(OBJ_DIR)/main.o

$(OBJ_DIR)/procesos.o: procesos.c $(OBJ_DIR)
	@echo "Compiling procesos.c..."
	cc -c procesos.c -o $(OBJ_DIR)/procesos.o

$(OBJ_DIR):
	@echo "Creating output directory..."
	mkdir -p $(OBJ_DIR)

clean:
	rm -fr $(OBJ_DIR)
	rm -f $(TARGET)