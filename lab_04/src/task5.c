// Вызов signal()

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include <sys/wait.h>
#include <sys/types.h>

#include <string.h>


#define TIME_FOR_SLEEP 5

#define TASK_TEXT "\n=======================\
                    \n     Task 5: signal()    \
                    \n=======================\n\n"


#define SIGNAL_TEXT "Зажмите: СTRL+\\ - вывести сообщения от потомков\n\n"


#define TEXT_CHILD_1 "Everybody wants to belive in miracles\n"
#define TEXT_CHILD_2 "Testing message\n"
#define TEXT_BUF 55
int flag = 0;



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


void catch_ctrlslash(int signal)
{
    flag = 1;

    printf("\nПойманный сигнал = %d\n", signal);
}

int main()
{
    printf(TASK_TEXT);

    char text1[TEXT_BUF] = "\0";
    char text2[TEXT_BUF] = "\0";

    pid_t child_pid;
    int status;

    int child1, child2;
    int fd[2];

    signal(SIGQUIT, catch_ctrlslash);

    printf(SIGNAL_TEXT);

    sleep(TIME_FOR_SLEEP);

    if (pipe(fd) == -1)
    {
        perror("Can not pipe\n");
        return -1;
    }

    if ((child1 = fork()) == -1)
    {
        perror("Can not fork\n");
        return -1;
    }
    else if (child1 == 0)
    {
        if (flag == 1)
        {
            close(fd[0]);
            write(fd[1], TEXT_CHILD_1, strlen(TEXT_CHILD_1) + 1);
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
        if (flag == 1)
        {
            close(fd[0]);
            write(fd[1], TEXT_CHILD_2, strlen(TEXT_CHILD_2) + 1);
        }

        exit(0);
    }

    if (child1 && child2)
    {
        close(fd[1]);
        
        read(fd[0], text1, strlen(TEXT_CHILD_1) + 1);
        read(fd[0], text2, strlen(TEXT_CHILD_2) + 1);

        printf("\nResult: %s\n", text1);
        printf("Result: %s\n\n", text2);

        child_pid = wait(&status);
        check_status(status);

        child_pid = wait(&status);
        check_status(status);
    }

    return 0;
}