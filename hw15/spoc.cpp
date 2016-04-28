//g++ -o spoc spoc.cpp -phread

#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdio.h>

using namespace std;

sem_t S,Sad,Sbd,Scd;


void* A(void* argv){          //烟草拥有者
    while(true){
        printf("Tabcco owner leave.\n");
        sem_wait(&Sad);
        printf("Tabcco owner come.\n");
        sem_post(&S);
    }
}

void* B(void* argv){          //纸拥有者
    while(true){
        printf("Paper owner leave.\n");
        sem_wait(&Sbd);
        printf("Paper owner come.\n");
        sem_post(&S);
    }
}

void* C(void* argv){          //火柴拥有者
    while(true){
        printf("Match owner leave.\n");
        sem_wait(&Scd);
        printf("Match owner come.\n");
        sem_post(&S);
    }
}

void* D(void* argv){          //经销商
    while(true){
        printf("Agency leave.\n");
        sem_wait(&S);
        printf("Agency come.\n");
        int op = time(NULL) % 3;
        switch (op) {
            case 0:
                printf("Agency puts paper and match on the table.\n");
                printf("Agency call tabcco owner.\n");
                sem_post(&Sad);
                break;
            case 1:
                sem_post(&Sbd);
                printf("Agency puts tabcco and match on the table.\n");
                printf("Agency call paper owner.\n");
                break;
            case 2:
                sem_post(&Scd);
                printf("Agency puts tabcco and paper on the table.\n");
                printf("Agency call match owner.\n");
                break;
            default:
                printf("Something error?\n");
        }
    }
}

int main(int argc, char* argv[]){
    sem_init(&S,0,1);
    sem_init(&Sad,0,0);
    sem_init(&Sbd,0,0);
    sem_init(&Scd,0,0);

    pthread_t thread[4];
    pthread_create(&thread[0],0,A,NULL);
    pthread_create(&thread[1],0,B,NULL);
    pthread_create(&thread[2],0,C,NULL);
    pthread_create(&thread[3],0,D,NULL);

    for(int i = 0;i < 3;i++){
        pthread_join(thread[i],0);
    }
    return 0;
}
