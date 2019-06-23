#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#define SHMKEY 75
  
int main() 
{ 

    //Declaración semáforo
    char *nombre;
    sem_t *semaphore;

    FILE *fp = fopen("Bitacora.txt", "w+");
    if(fp==NULL){
        printf("Error al abrir la Bitacora!\n");
        exit(1);
    }

    fprintf(fp, "%s", "Acciones de Memoria:");

    nombre = (char*)malloc(strlen("p_mutex")+1);
    snprintf(nombre,strlen("p_mutex")+1, "p_mutex");

    semaphore = sem_open(nombre, O_CREAT, S_IRWXU, 1);
    if(semaphore == SEM_FAILED){
        perror("semaphore initilization");
        exit(1);
    }

    char* bytesc;
    int bytes;
    printf("Ingrese la cantidad de bytes a reservar: ");
    scanf("%s", bytesc);

    bytes=atoi(bytesc)+1;

    // shmget returns an identifier in shmid 
    int shmid = shmget(SHMKEY,bytes+99,0777|IPC_CREAT); 

    if (shmid < 0) {
     printf("Error al obtener la memoria!\n");
     exit(1);
    }

    // shmat to attach to shared memory 
    char *memoria = (char*) shmat(shmid,(void*)0,0); 

    memoria[0]=bytes;

    printf("Obtención de memoria exitosa!...\n");
    printf("Memoria y Semáforo listos para uso!\n");

    //detach from shared memory  
    shmdt(memoria);


    //detach semaphore
    sem_close(semaphore);

    //close file
    fclose(fp);
  
    return 0; 
}
