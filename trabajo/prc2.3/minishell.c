#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include "orden.h"

#define BUFFER_SIZE 150

enum errors{
	SUCCESS,
	ERR_MORE_ARGS_THAN_EXP,
	ERR_NO_REDIRECT_ALLOWED,
	ERR_NO_SALIDA_ALLOWED,
	ERR_NO_ENTRADA_ALLOWED,
	ERR_COMMAND_NO_RECOGNIZED,
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

int check_errors(struct orden* o, int c_args, int c_args_skip, int c_red_in, 
					int c_red_out){
	if (o->args[c_args] != NULL && !c_args_skip){
		return ERR_MORE_ARGS_THAN_EXP;
	}
	if (c_red_in && c_red_out) {
		if(o->entrada!=NULL || o->salida!=NULL){
			return ERR_NO_REDIRECT_ALLOWED;
		}
	} else if (c_red_in) {
		if(o->entrada != NULL){
			return ERR_NO_ENTRADA_ALLOWED;
		}
	} else if (c_red_out) {
		if (o-> salida != NULL) {
			return ERR_NO_SALIDA_ALLOWED;
		}
	}
	return SUCCESS;
}

int exec_cd(struct orden* o){
	/*cd a b <-- error*/
	/*cd*/
	int resultado = check_errors(o, 2, 1, 1, 1); 
	if(resultado!=SUCCESS){
		return resultado;
	}
	if(o->args[1] == NULL){
		char *path;
		path = getenv("HOME");
		if (path == NULL) {
			fprintf(stderr, "%s", "La variable HOME no existe en este usuario");
			return ERROR;
		}
		if(chdir(path) != 0){
			perror("Error");
			return ERROR;
		}
		return SUCCESS;
	} else if(o->args[1] != NULL && o->args[2] == NULL){
		if(chdir(o->args[1]) != 0){
			perror("Error");
			return ERROR;
		}
	} else {
		return ERR_MORE_ARGS_THAN_EXP;
	}
	return SUCCESS;
}

int check_exit(struct orden* o){
	int resultado = check_errors(o, 1, 0, 1, 1); 
	return resultado;
}

int exec_pwd(struct orden* o){
	char path[BUFFER_SIZE];
	/*Cosas not allowed*/
	int resultado = check_errors(o, 1, 0, 1, 1);
	if(resultado!=SUCCESS){
		return resultado;
	}

	/*Errores de la función*/
	if(getcwd(path, BUFFER_SIZE) == NULL) {
		perror("Directorio actual no se pudo obtener");
		return ERROR;
	}

	printf("%s\n", path);
	return SUCCESS;
}

int exec_external_command(struct orden* actual) {
	
    int status;
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("Error al crear proceso hijo");
        return ERROR;
    }
    
    if (pid == 0) {
        if (execvp(actual->args[0], actual->args) == -1) {
            perror("Error al ejecutar el comando");
            exit(EXIT_FAILURE);
        }
    }
    
    if (waitpid(pid, &status, 0) == -1) {
        perror("Error esperando al proceso hijo");
        return ERROR;
    }
    
    if (WIFEXITED(status)) {
        if (WEXITSTATUS(status) == 0) {
            return SUCCESS;
        }
    }
    
    return ERROR;
}

int command_switcher(struct orden* actual){
	if(strcmp("pwd", actual->args[0]) == 0){
			return exec_pwd(actual);
		} else if(strcmp("cd", actual->args[0]) == 0){
			return exec_cd(actual);
		} else if(strcmp("exit", actual->args[0])== 0){
			return check_exit(actual);
		} else {
			/*return ERR_COMMAND_NO_RECOGNIZED; */
			return exec_external_command(actual);
		}
}

void show_error (enum errors error){
	switch (error)
	{
		case ERR_MORE_ARGS_THAN_EXP:
			fprintf(stderr, "%s\n", "More args than expected");
			break;
		case ERR_NO_REDIRECT_ALLOWED:
			fprintf(stderr, "%s\n", "Este comando no acepta redirección");
			break;
		case ERR_NO_SALIDA_ALLOWED:
			fprintf(stderr, "%s\n", "Este comando no acepta redirecciones de salida");
			break;
		case ERR_NO_ENTRADA_ALLOWED:
			fprintf(stderr, "%s\n", "Este comando no acepta redirecciones de entrada");
			break;
		case ERR_COMMAND_NO_RECOGNIZED:
			fprintf(stderr, "%s\n", "Comando no reconocido");
			break;
		default:
			break;
	}
}

int main(const int argc, const char* argv[])
{
	struct orden* actual;
	int resultado;

	do
	{
		actual = leer_orden("bt0354>");
		resultado = command_switcher(actual);
		if(resultado != SUCCESS) {
			show_error(resultado);
		}
		
	} while ((strcmp("exit", actual->args[0]) != 0) || (resultado != SUCCESS));
	printf("\n");
	return 0;
}
