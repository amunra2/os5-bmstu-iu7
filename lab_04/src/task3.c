// Вызов exec()

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/wait.h>
#include <sys/types.h>


#define TIME_FOR_SLEEP 3

#define TASK_TEXT "\n=======================\
                    \n     Task 3: exec()    \
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
    pid_t child_pid;
    int status;

    if ((child1 = fork()) == -1)
    {
        perror("Can not fork\n");
        return -1;
    }   
    else if (child1 == 0)
    {
        if (execlp("./tads_graph/tads_graph.exe", "./tads_graph.exe", NULL) == -1)
        {
            printf("\nError: Child 1 can not execute exec()\n");

            exit(0);
        }

        exit(0);
    }

    if ((child2 = fork()) == -1)
    {
        perror("Can not fork\n");
        return -1;
    }
    else if (child2 == 0)
    {
        if (execlp("./qsort/qsort.exe", "./qsort.exe", NULL) == -1)
        {
            printf("\nError: Child 2 can not execute exec()\n");

            exit(0);
        }

        exit(0);
    }

    child_pid = wait(&status);
    printf("\n\nProcess status: %d, child pid = %d\n", status, child_pid);
    check_status(status);

    child_pid = wait(&status);
    printf("Process status: %d, child pid = %d\n", status, child_pid);
    check_status(status);

    printf("\nParent: pid = %d, pgrp = %d\nChild1 = %d, Child2 = %d\n", getpid(), getpgrp(), child1, child2);

    return 0;
}
