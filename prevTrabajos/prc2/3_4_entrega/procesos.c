#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "procesos.h"

struct proceso 
{ 
   int bt0354_pid;
   char bt0354_ejecutable[MAX_EJECUTABLE];
   int bt0354_estado;
    
}; 
int anadir_proceso(struct proceso* procesos[], int ix, int pid,
                   char *ejecutable, int estado)
{
   struct proceso *procpointer;
   procpointer = calloc(1, sizeof(struct proceso));
   if (procpointer == NULL){
      perror("Whoops");
      return MEM_ERROR;
   }
   procpointer->bt0354_pid=pid;
   strncpy(procpointer->bt0354_ejecutable, ejecutable, MAX_EJECUTABLE);
   procpointer->bt0354_estado=estado;
   procesos[ix]=procpointer;

   return 0;
}
void listar_procesos(struct proceso* procesos[])
{
   int i;
   for(i=0;i<MAX_PROCESOS;i++){
      if(procesos[i]==NULL){
         printf("Posición de la tabla %i: Vacía\n", i);
      } else {
         /* printf("(%i) PID: %i - Exe: %s\n", procesos[i]->bt0354_estado, procesos[i]->bt0354_pid, procesos[i]->bt0354_ejecutable); */
         printf("Posición de la tabla %i: %i %s %i\n", i, procesos[i]->bt0354_pid, procesos[i]->bt0354_ejecutable, procesos[i]->bt0354_estado);
      }
   }
}
void liberar_procesos(struct proceso* procesos[])
{
   int i;
   for(i=0;i<MAX_PROCESOS;i++){
      if(procesos[i]!=NULL){
         free(procesos[i]);
      }
   }
   
}