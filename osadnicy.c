#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_DNI 365

int zwierzyna;
int pozywienie;

pthread_mutex_t pozywienie_m; // kontroluje dostęp do pożywienia
pthread_mutex_t zwierzyna_m; // kontroluje dostęp do zwierzyny

int millisecons_sleep = 10;

int rzut_kostka() {
    return rand() % 6 + 1;
}

void *mysliwy(void *ptr) {
    for(int i=1; i<=MAX_DNI; i++) {
        // polowanie
        if (rzut_kostka() > rzut_kostka()) {
            pthread_mutex_lock(&zwierzyna_m);
            zwierzyna++;
            pthread_mutex_unlock(&zwierzyna_m);
        }
        // powrót do osady
        pthread_mutex_lock(&pozywienie_m);
        if(pozywienie>0) {
            pozywienie--;
            pthread_mutex_unlock(&pozywienie_m);
        }
        else {
            (*((int*)ptr))--;
            pthread_mutex_unlock(&pozywienie_m);
            pthread_exit(NULL);
        }
        
        usleep(millisecons_sleep*1000);
    }
    pthread_exit(NULL);
}

void *kucharz(void *ptr) {

    for (int i=1;i<=MAX_DNI;i++) {
        //printf("zyje w dniu %d\n", i);
        int produkowac = 1;
        pthread_mutex_lock(&zwierzyna_m);
        if (zwierzyna > 0) {
            zwierzyna--;
        }
        
        else {
            produkowac = 0;
        }
        pthread_mutex_unlock(&zwierzyna_m);
        
        pthread_mutex_lock(&pozywienie_m);
        if (produkowac==1) {
            pozywienie+=rzut_kostka();
        }
        if (pozywienie>0) {
            pozywienie--;
            pthread_mutex_unlock(&pozywienie_m);
        }
        else {
            (*((int*)ptr))--;
            pthread_mutex_unlock(&pozywienie_m);
            pthread_exit(NULL);
        }
        
        
        usleep(millisecons_sleep*1000);
        
    }
    pthread_exit(NULL);
}


int main(int argc, char *argv[]) {
    srand(time(NULL));
    if(argc < 5) pthread_exit(0);

    int kucharze, mysliwi;

    sscanf(argv[1], "%d", &mysliwi);
    sscanf(argv[2], "%d", &kucharze);
    sscanf(argv[3], "%d", &zwierzyna);
    sscanf(argv[4], "%d", &pozywienie);
    
    pthread_mutex_init(&pozywienie_m, NULL);
    pthread_mutex_init(&zwierzyna_m, NULL);

    pthread_t *kuch, *mys;
    kuch = (pthread_t*)malloc(sizeof(pthread_t)*kucharze);
    mys = (pthread_t*)malloc(sizeof(pthread_t)*mysliwi);

    int k = kucharze;
    int m = mysliwi;
    
    for (int i=0;i<kucharze;i++) {
        pthread_create(&kuch[i], NULL, kucharz, (void*)&k);
    }
    for (int i=0;i<mysliwi;i++) {
        pthread_create(&mys[i], NULL, mysliwy, (void*)&m);
    }
    for (int i=0;i<kucharze;i++) {
        pthread_join(kuch[i], NULL);
    }
    for (int i=0;i<mysliwi;i++) {
        pthread_join(mys[i], NULL);
    }

    free(kuch);
    free(mys);
    
    pthread_mutex_destroy(&pozywienie_m);
    pthread_mutex_destroy(&zwierzyna_m);

    
    printf("KONIEC\nil mysliwych: %d\nil kucharzy: %d\nil pozywienia: %d\nil zwierzyny: %d\n", m, k, pozywienie, zwierzyna);

    pthread_exit(NULL);
}
