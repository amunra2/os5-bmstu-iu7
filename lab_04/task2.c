// Вызов wait()

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/wait.h>
#include <sys/types.h>

#define OK 0

#define ERROR -1
#define ERR_FORK -1

#define TIME_FOR_SLEEP 3


#define TASK_TEXT "\n=======================\
                   \n     Task 2: wait()    \
                   \n=======================\n\n"


void check_status(int status)
{
    if (WIFEXITED(status))
    {
        printf("Дочерний процесс завершен корректно\n");
        printf("Дочерний процесс завершился с кодом: \t%d\n\n", WEXITSTATUS(status));

        return;
    }

    if (WIFSIGNALED(status))
    {
        printf("Дочерний процесс завершен неперехватываемым сигналом\n");
        printf("Номер сигнала: \t%d\n\n", WTERMSIG(status));

        return;
    }

    if (WIFSTOPPED(status))
    {
        printf("Дочерний процесс остановлен\n");
        printf("Номер сигнала: \t%d\n\n", WSTOPSIG(status));

        return;
    }
}


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


    // Порождение процесса-потомка через fork()
    if ((child2 = fork()) == ERR_FORK)
    {
        perror("Can not fork\n");
        return ERROR;
    }
    else if (child2 == OK)
    {
        sleep(TIME_FOR_SLEEP);

        printf("\nChild 2: pid = %d, ppid = %d, pgrp = %d\n\n", getpid(), getppid(), getpgrp());

        exit(OK);
    }

    // wait() - блокирует родительский процесс до момента завершения дочернего
    pid_t child_pid;
    int status;

    child_pid = wait(&status);
    printf("Process status: %d, child pid = %d\n", status, child_pid);
    check_status(status);

    child_pid = wait(&status);
    printf("Process status: %d, child pid = %d\n", status, child_pid);
    check_status(status);

    printf("\nParent: pid = %d, pgrp = %d\nChild1 = %d, Child2 = %d\n", getpid(), getpgrp(), child1, child2);

    return OK;
}