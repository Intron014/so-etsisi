TARGET = minishell
OBJS = orden.o minishell.o

all: $(TARGET)

$(TARGET): $(OBJS) synced
	rm synced
	cc -o $(TARGET) $(OBJS)

orden.o: orden.c orden.h synced
	cc -c orden.c

minishell.o: minishell.c synced
	cc -c minishell.c

synced:
	syncwin
	touch synced

clean:
	rm -f $(OBJS) $(TARGET)
	rm -f synced