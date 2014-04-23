#include <unistd.h>  
#include <stdlib.h>  
#include <stdio.h>  
  
#include <sys/types.h>  
#include <sys/ipc.h>  
#include <sys/sem.h>  
  
#include "semun.h"  
  
static int set_semvalue(void);  
static void del_semvalue(void);  
static int semaphore_p(void);  
static int semaphore_v(void);  
  
static int sem_id;  
  
  
int main(int argc, char *argv[])  
{  
    int i;  
    int pause_time;  
    char op_char = 'O';  
  
    srand((unsigned int)getpid());  
      
    sem_id = semget((key_t)1234, 1, 0666 | IPC_CREAT);  
  
    if (argc > 1) {  
        if (!set_semvalue()) {  
            fprintf(stderr, "Failed to initialize semaphore\n");  
            exit(EXIT_FAILURE);  
        }  
        op_char = 'X';  
        sleep(2);  
    }  
	
	//first make a call to semaphore_p, which sets the semaphore to wait as this program is about to enter the critical section
	
	for(i = 0; i < 10; i++) {          
  
        if (!semaphore_p()) exit(EXIT_FAILURE);  
        printf("%c", op_char);fflush(stdout);  
        pause_time = rand() % 3;  
        sleep(pause_time);  
        printf("%c", op_char);fflush(stdout);  
		
		//after the critical section, you call semaphore_v, setting the semaphore as available
		 if (!semaphore_v()) exit(EXIT_FAILURE);  
          
        pause_time = rand() % 2;  
        sleep(pause_time);  
    }      
  
    printf("\n%d - finished\n", getpid());  
  
    if (argc > 1) {      
        sleep(10);  
        del_semvalue();  
    }  
          
    exit(EXIT_SUCCESS);  
}  

//sem_semvalue initializes the semaphore using SETVAL command in a semctl call
static int set_semvalue(void)  
{  
    union semun sem_union;  
  
    sem_union.val = 1;  
    if (semctl(sem_id, 0, SETVAL, sem_union) == -1) return(0);  
    return(1);  
}  

//del_semvalue function has almost the same form, except that the call to semctl uses the command IPC_RMID to remove the semaphore's ID
static void del_semvalue(void)  
{  
    union semun sem_union;  
      
    if (semctl(sem_id, 0, IPC_RMID, sem_union) == -1)  
        fprintf(stderr, "Failed to delete semaphore\n");  
}  

//semaphore_p changes the semaphore by -1. This is the wait operation
static int semaphore_p(void)  
{  
    struct sembuf sem_b;  
      
    sem_b.sem_num = 0;  
    sem_b.sem_op = -1; /* P() */  
    sem_b.sem_flg = SEM_UNDO;  
    if (semop(sem_id, &sem_b, 1) == -1) {  
        fprintf(stderr, "semaphore_p failed\n");  
        return(0);  
    }  
    return(1);  
} 

//semaphore_v is similar except for setting the sem_op part of the sembuf structure to 1
static int semaphore_v(void)  
{  
    struct sembuf sem_b;  
      
    sem_b.sem_num = 0;  
    sem_b.sem_op = 1; /* V() */  
    sem_b.sem_flg = SEM_UNDO;  
    if (semop(sem_id, &sem_b, 1) == -1) {  
        fprintf(stderr, "semaphore_v failed\n");  
        return(0);  
    }  
    return(1);  
}  










































