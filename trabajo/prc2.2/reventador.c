#define _MINIX

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include "generador.h"
#include "probar-clave.h"
#include <string.h>
#include <signal.h>

#define MAX_WORKERS 100
pid_t stack_trabajadores[MAX_WORKERS];
int num_active_workers = 0;
int cfound = 0;

void manejador(int signum) {
    exit(2);
}

static void add_trabajador(pid_t pid) {
    if (num_active_workers < MAX_WORKERS) {
        stack_trabajadores[num_active_workers++] = pid;
    }
}

static void remove_trabajador(pid_t pid) {
	int i;
    for (i = 0; i < num_active_workers; i++) {
        if (stack_trabajadores[i] == pid) {
            stack_trabajadores[i] = stack_trabajadores[--num_active_workers];
            break;
        }
    }
}

static void kill_them_all(void) {
	int i;
    for (i = 0; i < num_active_workers; i++) {
        kill(stack_trabajadores[i], SIGTERM);
    }
}

/**
 * Muestra todas las palabras que puede genera el generador.
 * Es importante, para los pasos siguientes, entender cómo se
 * reserva memoria para recoger la palabra y cómo se libera,
 * porque será necesario aplicarlo en el paso 5 de la práctica.
 */
static int escribir_palabras(void)
{
	int i;
	char* const palabra = calloc(longitud()+2, sizeof(char));

	for (i = 0; i < total_palabras(); i++)
	{
		if (genera_palabra(i, palabra) != OK)
		{
			return !OK;
		}
		fprintf(stdout, "%s\n", palabra);
	}
	free(palabra);
	return OK;
}

static void print_salt(const char salted_hash[]){
	char notif[80];
	int thewr;
	char salt[3];

	strncpy(salt, salted_hash, 2);
	salt[2]='\0';

	thewr = snprintf(notif, 80, "pid: %d - salt_hash: %s - salt: %s\n", 
						getpid(), salted_hash, salt);

	if(write(1, notif, thewr) == -1){
		perror("Mensaje no impreso");
	}
}

static int print_words(const int f, const int l, const char salted_hash[]) {
	int index;
	int lon;
	int cfound;
	char salt[3];
	char* const word = calloc(longitud(), sizeof(char));
	print_salt(salted_hash);
	strncpy(salt, salted_hash, 2);
	salt[2]='\0';
	index = f;
	cfound = 0;
	while (index <= l && !cfound){
		if(genera_palabra(index, word) != OK) {
			return !OK;
		}
		if(!probar_clave(word, salted_hash, salt)){
			char notif[80];
			int thewr;
			thewr = snprintf(notif, 80, "[%d] Clave encontrada: %s\n", getpid(), word);
			cfound = 1;
			if(write(1, notif, thewr) == -1){
				perror("Mensaje no impreso");
			}
		}
		index++;
	}
	free(word);
	if(!cfound){
		char notif[80];
		int thewr;
		thewr = snprintf(notif, 80, "[%d] No se ha encontrado la clave\n", getpid());
		if(write(1, notif, thewr) == -1){
			perror("Mensaje no impreso");
		}
		return !OK;
	}
	return OK;
}

static void print_pid(const int f, const int l){
	char notif[45];
	int thewr;

	thewr = snprintf(notif, 45, "PID: %d - [%d, %d]\n", getpid(), f, l);

	if(write(1, notif, thewr) == -1){
		perror("Mensaje no impreso");
	}
}

static int dealer(const int num_procesos, const char salted_hash[]) {
	int word;
	int proc;
	int tot_pal;
	int base_ppg;
	int resto;
	int start_word;
	int end_word;
	int words_for_proc;
	int son;
	pid_t pid = 1;

	word = 0;
	tot_pal = total_palabras();
	base_ppg = tot_pal / num_procesos;
	resto = tot_pal % num_procesos;

	signal(SIGTERM, manejador);

	for (proc = 0; proc < num_procesos && word < tot_pal && pid != 0 ; proc++) {
		start_word = word;
		words_for_proc = base_ppg + (proc < resto ? 1 : 0);
		end_word = start_word + words_for_proc - 1;
		if (words_for_proc > 0) {
			pid = fork();
			if (pid == -1) {
				perror("Proceso hijo no creado");
			}

			if(pid != 0){
				word +=words_for_proc;
			}
		}

		if (pid == 0) {
            exit(print_words(start_word, end_word, salted_hash));
        } else if (pid > 0) {
            add_trabajador(pid);
        }
	}

	if (pid != 0) {
        int status;
        pid_t finished_pid;
        while ((finished_pid = waitpid(-1, &status, 0)) != -1) {
            remove_trabajador(finished_pid);
            
            if (WIFEXITED(status)) {
                int exit_status = WEXITSTATUS(status);
                if (exit_status == 0) {
                    cfound = 1;
                    kill_them_all();
                } else if (exit_status == 2) {
                    printf("[%d] Abortado (SIGTERM)\n", finished_pid);
                }
            } else if (WIFSIGNALED(status)) {
                printf("[%d] Abortado (%d)\n", finished_pid, WTERMSIG(status));
            }
        }

        if (cfound) {
            printf("Controlador: clave encontrada\n");
        } else {
            printf("Controlador: clave no encontrada\n");
        }
    }
    return OK;
}


static void escribir_numword(const int num_procesos) {
	int word;
	int proc;
	int tot_pal;
	int base_ppg;
	int resto;
	int start_word;
	int words_for_proc;

	word = 0;
	tot_pal = total_palabras();
	base_ppg = tot_pal / num_procesos;
	resto = tot_pal % num_procesos;

	for (proc = 0; proc < num_procesos && word < tot_pal; proc++) {
		start_word = word;
		words_for_proc = base_ppg + (proc < resto ? 1 : 0);

		if (words_for_proc > 0) {
			fprintf(stdout, "Proc %i - [%i, %i]\n", proc, start_word, start_word 
												+ words_for_proc - 1);
			word += words_for_proc;
		}
	}
}




/* Realiza el trabajo del reventador una vez que se ha obtenido la
 * información necesaria de los argumentos del programa.
 * Si todo va bien, devuelve OK. En caso contrario devuelve !OK.
 */
static int trabajar(const char alfabeto[], const int longitud, const int num_procesos, 
						const char salted_hash[])
{
	int ppg;
	if (configura_generador(alfabeto, longitud) != OK)
	{
		return !OK;
	}
	
	return dealer(num_procesos, salted_hash);
}

/**
 * Muestra un mensaje indicando cómo debe usarse el programa. Si se
 * modifican los argumentos del programa, hay que modificar esta función.
 */
static void uso(const char prog[])
{
	fprintf(stderr, "Uso: %s <alfabeto> <longitud> <num procesos> <salted hash>\n", prog);
}

/**
 * Dada una cadena de caracteres, obtiene su representación numérica entera.
 * Tiene los siguientes parámetros:
 *	cadena: la versión textual del número.
 *	num: referencia al entero que albergará el valor numérico de la cadena.
 * Si cadena contiene una cadena numérica válida, devuelve OK.
 * En caso contrario, devuelve !OK y el valor referenciado por num es
 * indeterminado.
 */
static int str2int(const char cadena[], long *num)
{
	char* endptr;

	errno = OK;
	*num = strtol(cadena, &endptr, 10);
	if (errno != OK || *endptr != '\0' || *num < 1)
	{
		return !OK;
	}
	return OK;
}

int main(const int argc, const char * argv[])
{
	long longitud;
	long numero_procesos;

	if (argc != 5)
	{
		uso(argv[0]);
		return !OK;
	}
	if (str2int(argv[3], &numero_procesos) != OK)
	{
		fprintf(stderr, "los hijos no son válidos\n");
		return !OK;
	}

	if (str2int(argv[2], &longitud) != OK)
	{
		fprintf(stderr, "la longitud no es válida\n");
		return !OK;
	}
	
	if (trabajar(argv[1], longitud, numero_procesos, argv[4]) != OK)
	{
		fprintf(stderr, "el programa ha fallado\n");
		return !OK;
	}
	return OK;
}

