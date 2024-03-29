#include "threadtools.h"
#include <sys/signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void fibonacci(int id, int arg) {
    thread_setup(id, arg);
    for (RUNNING->i = 1; ; RUNNING->i++) {
        if (RUNNING->i <= 2)
            RUNNING->x = RUNNING->y = 1;
        else {
            /* We don't need to save tmp, so it's safe to declare it here. */
            int tmp = RUNNING->y;
            RUNNING->y = RUNNING->x + RUNNING->y;
            RUNNING->x = tmp;
        }
        printf("%d %d\n", RUNNING->id, RUNNING->y);
        sleep(1);
        fprintf(stderr, "i: %d arg: %d\n", RUNNING->i, RUNNING->arg);
        if (RUNNING->i == RUNNING->arg) {
            thread_exit();
        }
        else {
            thread_yield();
            
        }
    }
}

void factorial(int id, int arg){
    thread_setup(id, arg);

    for(RUNNING->i = 1,RUNNING->x = 1; ; RUNNING->i++){
        RUNNING->x *= RUNNING->i;
        printf("%d %d\n", RUNNING->id, RUNNING->x);
        sleep(1);
        fprintf(stderr, "i: %d arg: %d\n", RUNNING->i, RUNNING->arg);
        if (RUNNING->i == RUNNING->arg) {
            thread_exit();
        }
        else {
            thread_yield();
        }
    }
}


void bank_operation(int id, int arg) {
    thread_setup(id, arg);
    lock();
    printf("%d acquired the lock\n", RUNNING->id);
    sleep(1);
    thread_yield();
    RUNNING->x = bank.balance;
    if(RUNNING->arg > 0){
        bank.balance += RUNNING->arg;
    }
    else if(RUNNING->arg < 0 && bank.balance >= abs(RUNNING->arg)){
        bank.balance += RUNNING->arg;
    }
    printf("%d %d %d\n", RUNNING->id, RUNNING->x, bank.balance);
    sleep(1);
    thread_yield();
    unlock();
    printf("%d released the lock\n", RUNNING->id);
    sleep(1);
    thread_exit();
}