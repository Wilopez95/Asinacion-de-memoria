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

int main(){

    char *nombre;
    sem_t *semaphore;

    printf("Limpiando...\n");

    int shmid = shmget(SHMKEY,1,0777);
    if (shmid < 0) {
     printf("Error al obtener la memoria!\n");
     exit(1);
    } 

    char *memoria = (char*) shmat(shmid,(void*)0,0); 

    nombre = (char*)malloc(strlen("p_mutex")+1);
    snprintf(nombre,strlen("p_mutex")+1, "p_mutex");

    semaphore = sem_open(nombre, O_CREAT, S_IRWXU, 1);
    if(semaphore == SEM_FAILED){
        perror("semaphore initilization");
        exit(1);
    }

    sem_wait(semaphore);
    memoria[0]=0;
    sem_post(semaphore);

    sleep(1);

    // destroy the shared memory, semaphore, file and stop process

    shmctl(shmid,IPC_RMID,NULL);
    sem_close(semaphore); 
    sem_unlink(nombre);

    printf("Memoria devuelta, Semáforo desactivado y\nProcesos matados con éxito!\n");

    return 0;
}