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
#define FILEKEY 70

//Prototipos

void *ProcesoNuevo(void *arg);
int FirstFit(int tam, int id);
int BestFit(int tam, int id);
int WorstFit(int tam, int id);
int Cabe(int tam, int id);

//************************

pthread_t tid[99];
sem_t *semaphore;
char *memoria;
int bytes;
char opcion=0;
FILE *fp;

void *ProcesoNuevo(void *arg){

    int id = *(int*)arg;
    int tam= rand() % 8 + 1;
    int time= rand() % 41 + 20;
    printf("%s%d%s%d%s%d\n", "id:", id, " t: ", time, " m: ", tam);

    int ubicacion;
    if(opcion==1) ubicacion=FirstFit(tam, id);
    if(opcion==2) ubicacion=WorstFit(tam, id);
    if(opcion==3) ubicacion=BestFit(tam, id);

    if(ubicacion==-1){
        sem_wait(semaphore);
        fprintf(fp, "\nEL proceso, PID: %d no entro a memoria", id);
        memoria[bytes+id]=0;
        sem_post(semaphore);
    }

    else{
        sleep(time);

        sem_wait(semaphore);
        for (int i = ubicacion; i < ubicacion+tam+1; i++)memoria[i]=0;
        fprintf(fp, "\nDesasignacion de %d Bytes para PID: %d", tam, id);
        memoria[bytes+id]=0;
        sem_post(semaphore);
    }
}

int Cabe(int tam, int id)
{
    for (int i = id; i < id+tam; i++)
    {
        if(memoria[i]!=0)return i;
    }

    return -1;
}

int FirstFit(int tam,int id){

    sem_wait(semaphore);
    
    int busy;
    for (int i = 1; i < bytes; i++){
        if(memoria[i]==0 && i+tam < bytes){
            busy=Cabe(tam, i); 
            if(busy==-1){
                for (int j = i; j < i+tam; j++){
                    memoria[j]=id;
                }

                fprintf(fp, "\nAsignacion de %d Bytes para PID: %d", tam, id);
                memoria[bytes+id]=1;
                sem_post(semaphore);

                return i;
            }
            i=busy;
        }
    }

    sem_post(semaphore);

    return -1;
}

int BestFit(int tam,int id){

    sem_wait(semaphore);
    
    int busy;
    int best=-1;
    int extras=bytes-tam;
    for (int i = 1; i < bytes; i++){
        if(memoria[i]==0 && i+tam < bytes){
            busy=Cabe(tam, i); 
            if(busy==-1){
                for (int j=i+tam; j < bytes; j++)
                {
                    if((memoria[j]!=0 || j==bytes-1) && abs(extras>i+tam-j))
                    {
                        extras=i+tam-j;
                        best=i;
                        break;
                    }
                }
            }
            else i=busy;
        }
    }

    if(best!=-1)
    {
        for (int i = best; i < best+tam; i++){
            memoria[i]=id;
        }

        fprintf(fp, "\nAsignacion de %d Bytes para PID: %d", tam, id);
        memoria[bytes+id]=1;
        sem_post(semaphore);

        return best;
    }

    sem_post(semaphore);

    return -1;
}

int WorstFit(int tam,int id){

    sem_wait(semaphore);
    
    int busy;
    int worst=-1;
    int extras=-1;
    for (int i = 1; i < bytes; i++){
        if(memoria[i]==0 && i+tam < bytes){
            busy=Cabe(tam, i); 
            if(busy==-1){
                for (int j=i+tam; j < bytes; j++)
                {
                    if((memoria[j]!=0 || j==bytes-1) && abs(extras>i+tam-j))
                    {
                        extras=i+tam-j;
                        worst=i;
                        break;
                    }
                }
            }
            else i=busy;
        }
    }

    if(worst!=-1)
    {
        for (int i = worst; i < worst+tam; i++){
            memoria[i]=id;
        }

        fprintf(fp, "\nAsignacion de %d Bytes para PID: %d", tam, id);
        memoria[bytes+id]=1;
        sem_post(semaphore);

        return worst;
    }

    sem_post(semaphore);

    return -1;
}

int main() 
{

    fp = fopen("Bitacora.txt", "a+t");
    if(fp==NULL){
        printf("Error al abrir la Bitacora!\n");
        exit(1);
    }

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

    char *opciones;
    while(opcion!=1 && opcion!=2 && opcion!=3)
    {
        printf("Digite el modo de memoria:\n 1. FF\n 2. WF\n 3. BF\n -:");
        scanf("%s", opciones);
        opcion=atoi(opciones);
    }

    printf("\nProcesos:\n");

    int tiempo;

    for (int i = 1; i < 99; i++)
    {
        sem_wait(semaphore);
        if(memoria[0]==0) break;
        memoria[bytes+i]=2;
        sem_post(semaphore);

        tiempo= rand() % 31 + 30;
        pthread_create(&(tid[i]), NULL, ProcesoNuevo, &i);
        sleep(tiempo);
    }

    //detach from shared memory and semaphore and file
    sem_close(semaphore);
    shmdt(memoria);
    fclose(fp);

    return 0; 
}
