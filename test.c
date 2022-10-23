#include "qsh.h"
#include <pthread.h>
#include <unistd.h>

void *qsh_main(void *arg){
    qsh_output("hello, qsh!\n");
    qsh_entry(NULL);
    return NULL;
}

int main(int argc, char **argv){
    pthread_t tid;
    pthread_create(&tid, NULL, qsh_main, NULL);
    int i = 0;
    while(1){
        sleep(1);
        // qsh_output("current: %d\n", i++);
    }
    return 0;
}


