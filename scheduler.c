#include "threadtools.h"
#include <sys/signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
 * Print out the signal you received.
 * If SIGALRM is received, reset the alarm here.
 * This function should not return. Instead, call siglongjmp(sched_buf, 1).
 */
void sighandler(int signo) {
    // TODO
    if(signo == SIGALRM){
        printf("caught SIGALRM\n");
        alarm(timeslice);
    }
    else if(signo == SIGTSTP){
        printf("caught SIGTSTP\n");
    }
    siglongjmp(sched_buf, 1);
}


/*
 * Prior to calling this function, both SIGTSTP and SIGALRM should be blocked.
 - `longjmp(sched_buf, 1)` from `sighandler` triggered by `thread_yield`
 - `longjmp(sched_buf, 2)` from `lock`
 - `longjmp(sched_buf, 3)` from `thread_exit`
 */
void scheduler() {
    // TODO
    rq_current = 0;
    int option = sigsetjmp(sched_buf, 1);
    fprintf(stderr,"in scheduler %d\n", option);
    if(bank.lock_owner == -1 && wq_size >= 1){
        fprintf(stderr,"its not happen\n");
        rq_size++;
        ready_queue[rq_size - 1] = waiting_queue[0];
        // fill the holes
        for(int i = 1; i < wq_size; i++){
            waiting_queue[i-1] = waiting_queue[i];
        }
        wq_size--;
    }

    if(option == 1){
        rq_current++;
        if(rq_current == (rq_size)) 
            rq_current = 0;
        fprintf(stderr,"thread yield  rq_current: %d rq_size: %d\n", rq_current, rq_size);
    }
    else if(option == 2){
        wq_size++;
        waiting_queue[wq_size - 1] = ready_queue[rq_current];
        if(rq_current == rq_size -1){
            rq_current = 0;
        }
        else{
            ready_queue[rq_current] = ready_queue[rq_size - 1];
        }
        rq_size--;
    }
    else if(option == 3){
        // ready_queue[rq_current] = ready_queue[rq_size];
        free(ready_queue[rq_current]);
        // printf("rq_curr: %d rq_size %d\n", rq_current, rq_size);
        if(rq_current == rq_size -1){
            rq_current = 0;
        }
        else{
            ready_queue[rq_current] = ready_queue[rq_size - 1];
        }
        rq_size--;
    }
    if(rq_size == 0 && wq_size == 0){
        return;
    }
    else{
        siglongjmp(ready_queue[rq_current]->environment, 1);
    }
}

    