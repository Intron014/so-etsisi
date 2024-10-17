#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "orden.h"

#define BUFFER_SIZE 150

enum errors{
	SUCCESS,
	ERROR
};

void mostrar_orden(struct orden* o)
{
	int i;

	for (i = 0; o->args[i] != NULL; i++)
	{
		printf("%s ", o->args[i]);
	}
	if (o->entrada != NULL)
	{
		printf("< %s ", o->entrada);
	}
	if (o->salida != NULL)
	{
		printf("> %s ", o->salida);
	}
	printf ("\n");
}

int exec_cd(struct orden* o){
	if (o->args[2] != NULL){
		fprintf(stderr, "%s\n", "More args than expected");
	}
	chdir(o->args[1]);
}

int check_exit(struct orden* o){

}

int exec_pwd(struct orden* o){
	char path[BUFFER_SIZE];
	if(o->args[1]!=NULL){
		fprintf(stderr, "%s\n", "More args than expected");
		return ERROR;
	}

	if(o->entrada!=NULL || o->salida!=NULL){
		fprintf(stderr, "%s\n", "Este comando no acepta redirección");
		return ERROR;
	}

	if(getcwd(path, BUFFER_SIZE) == NULL) {
		fprintf(stderr, "%s\n", "Something happened");
		return ERROR;
	}
	printf("%s\n", path);
	return SUCCESS;
}

int main(const int argc, const char* argv[])
{
	struct orden* actual;

	do
	{
		actual = leer_orden("bt0354>");
		if(strcmp("pwd", actual->args[0]) == 0){
			exec_pwd(actual);
		} else if(strcmp("cd", actual->args[0]) == 0){
			exec_cd(actual);
		} else if(strcmp("exit", actual->args[0])== 0){
			check_exit(actual);
		} else {
			fprintf(stderr, "%s\n", "Comando no reconocido");
		}
		
	} while (strcmp("exit", actual->args[0]) != 0);
	return 0;
}
