#define _MINIX

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include "generador.h"
#include <string.h>

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

static int print_words(const int f, const int l) {
	int index;
	int lon;
	char* const word = calloc(longitud()+2, sizeof(char));

	index = f;
	while (index <= l){
		if(genera_palabra(index, word) != OK) {
			return !OK;
		}
		lon = strlen(word);
		word[lon] = '\n';
		word[lon + 1] = '\0';
		if (write(1, word, strlen(word)) == -1){
			perror("Whoops, el mensaje se perdió");
		}
		index++;
	}
	free(word);
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

static void dealer(const int num_procesos) {
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
	}

	if (pid != 0){
		while (wait(&son) != -1);
	} else {
		print_words(start_word, end_word);
	}

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
static int trabajar(const char alfabeto[], const int longitud, const int num_procesos)
{
	int ppg;
	if (configura_generador(alfabeto, longitud) != OK)
	{
		return !OK;
	}
	
	dealer(num_procesos);
	return OK;
}

/**
 * Muestra un mensaje indicando cómo debe usarse el programa. Si se
 * modifican los argumentos del programa, hay que modificar esta función.
 */
static void uso(const char prog[])
{
	fprintf(stderr, "Uso: %s <alfabeto> <longitud> <num procesos>\n", prog);
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

	if (argc != 4)
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

	

	if (trabajar(argv[1], longitud, numero_procesos) != OK)
	{
		fprintf(stderr, "el programa ha fallado\n");
		return !OK;
	}
	return OK;
}

