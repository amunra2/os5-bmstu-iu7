// Процессы-сирота

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define OK 0

#define ERROR -1
#define ERR_FORK -1

#define TIME_FOR_SLEEP 3


#define TASK_TEXT "\n=======================\
                   \n     Task 1: fork()    \
                   \n=======================\n\n"


int main()
{
    printf(TASK_TEXT);
    
    int child1, child2;

    // Порождение процесса-потомка через fork()
    if ((child1 = fork()) == ERR_FORK)
    {
        perror("Can not fork\n");
        return ERROR;
    }
    else if (child1 == OK)
    {
        sleep(TIME_FOR_SLEEP);

        printf("\nChild 1: pid = %d, ppid = %d, pgrp = %d\n", getpid(), getppid(), getpgrp());

        exit(OK);
    }


    if ((child2 = fork()) == ERR_FORK)
    {
        perror("Can not fork\n");
        return ERROR;
    }
    else if (child2 == OK)
    {
        sleep(TIME_FOR_SLEEP);

        printf("\nChild 2: pid = %d, ppid = %d, pgrp = %d\n", getpid(), getppid(), getpgrp());

        exit(OK);
    }


    printf("\nParent: pid = %d, pgrp = %d\nChild1 = %d, Child2 = %d\n", getpid(), getpgrp(), child1, child2);

    return OK;
}