#define _MINIX
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include "generador.h"

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

static int escribir_numword(const int ppg, const int num_procesos){
	int word;
	int proc;
	int tot_pal;
	tot_pal=total_palabras();
	fprintf(stdout, "TOTPAL: %i\nPPG: %i\n", tot_pal, ppg);
	for (proc = 0, word = 1; proc<num_procesos && word <= tot_pal; proc++){
		fprintf(stdout, "Proc %i - [%i", proc, word);
		if(word < tot_pal && word+ppg < tot_pal) {
			fprintf(stdout, ", %i]\n", word+(ppg-1));
			word+=ppg;
		} else if(word < tot_pal && word+ppg == tot_pal && proc == num_procesos-1) {
			fprintf(stdout, ", %i]\n", word+=ppg);
		} else if(word < tot_pal && word+ppg >= tot_pal && proc == num_procesos-1) {
			fprintf(stdout, ", %i]\n", tot_pal);
			word=tot_pal+1;
		} else {
			fprintf(stdout, ", %i]\n", word+(ppg-1));
			word+=ppg;
		}
	}
	return OK;
}

static int create_sons(const int ppg, const int num_procesos){
	int word;
	int proc;
	int tot_pal;
	tot_pal=total_palabras();
	fprintf(stdout, "TOTPAL: %i\nPPG: %i\n", tot_pal, ppg);
	for (proc = 0, word = 1; proc<num_procesos && word <= tot_pal; proc++){
		fprintf(stdout, "Proc %i - [%i", proc, word);
		if(word < tot_pal && word+ppg < tot_pal) {
			fork();
			word+=ppg;
		} else if(word < tot_pal && word+ppg == tot_pal && proc == num_procesos-1) {
			fprintf(stdout, ", %i]\n", word+=ppg);
		} else if(word < tot_pal && word+ppg >= tot_pal && proc == num_procesos-1) {
			fprintf(stdout, ", %i]\n", tot_pal);
			word=tot_pal+1;
		} else {
			fprintf(stdout, ", %i]\n", word+(ppg-1));
			word+=ppg;
		}
	}
	return OK;
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
	if ((total_palabras()/num_procesos)%2==0 && (total_palabras()/num_procesos)*num_procesos!=num_procesos){
		ppg = (total_palabras()/num_procesos) + 1;
	} else {
		ppg = (total_palabras()/num_procesos);
	}
	
	return escribir_numword(ppg, num_procesos);
	/*return escribir_palabras(); */
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
	printf("Procesos a crear: %li\n", numero_procesos);

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

