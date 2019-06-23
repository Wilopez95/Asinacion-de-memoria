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

//************************
sem_t *semaphore;
char *memoria;
int bytes;

int main() 
{
    //Suscripción al semaforo
    char *nombre;

    nombre = (char*)malloc(strlen("p_mutex")+1);
    snprintf(nombre,strlen("p_mutex")+1, "p_mutex");

    semaphore = sem_open(nombre, O_CREAT, S_IRWXU, 1);
    if(semaphore == SEM_FAILED){
        perror("semaphore initilization");
        exit(1);
    }

    // shmget returns an identifier in shmid 
    int shmid = shmget(SHMKEY,1,0777);
    if (shmid < 0) {
     printf("Error al obtener la memoria!\n");
     exit(1);
    } 
  
    // shmat to attach to shared memory 
    memoria = (char*) shmat(shmid,(void*)0,0); 

    bytes=memoria[0];

    // shmget returns an identifier in shmid 
    shmid = shmget(SHMKEY,bytes+99,0777); 
  
    // shmat to attach to shared memory 
    memoria = (char*) shmat(shmid,(void*)0,0);

    printf("Matriz de celdas de Memoria: \n");
    sem_wait(semaphore);
    for (int j = 1; j < bytes; j++)
    {   
        if(memoria[j]>9) printf("%c%d%c", '[', memoria[j], ']');
        else printf("%s%d%c", "[0", memoria[j], ']');

        if((j)%8==0 && j!=0) printf("\n");
    }

    printf("\nProcesos en ejecucion: \n");

    for (int j = bytes+1; j < bytes+99; j++)
    {   
        if(memoria[j]==1) printf("%d-", j-bytes);
    }

    printf("\n\nProcesos en bloqueo: \n");

    for (int j = bytes+1; j < bytes+99; j++)
    {   
        if(memoria[j]==2) printf("%d-", j-bytes);
    }

    sem_post(semaphore);

    printf("\n");

    //detach from shared memory  
    sem_close(semaphore);
    shmdt(memoria);

    return 0; 
}
