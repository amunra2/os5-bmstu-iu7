// Вызов wait()

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include <sys/wait.h>
#include <sys/types.h>

#include <string.h>

#define OK 0

#define ERROR -1
#define ERR_FORK -1
#define ERR_PIPE -1

#define TIME_FOR_SLEEP 15

#define TASK_TEXT "\n=======================\
                   \n     Task 5: signal()    \
                   \n=======================\n\n"


#define SIGNAL_TEXT "Зажмите: СTRL+\\ - вызвать сообщение от потомка 1 \
                            \nCtrl+C - вызвать сообщение от потомка 2\n\n"


#define TEXT_CHILD_1 "Child 1 is texting  message\n"
#define TEXT_CHILD_2 "Child 2 is printing message\n"
#define TEXT_BUF 55

#define READ 0
#define WRITE 1

#define CTRL_SLASH 1
#define CTRL_C 0
#define NOTHING 2
int flag = NOTHING;



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



void catch_ctrlc(int signal)
{
    flag = CTRL_C;

    printf("\nПойманный сигнал = %d\n", signal);
}


void catch_ctrlslash(int signal)
{
    flag = CTRL_SLASH;

    printf("\nПойманный сигнал = %d\n", signal);
}



int main()
{
    printf(TASK_TEXT);

    // Перехват сигналов
    signal(SIGINT, catch_ctrlc);
    signal(SIGQUIT, catch_ctrlslash);

    int fd[2]; // 0 - чтение, 1 - запись

    if (pipe(fd) == ERR_PIPE)
    {
        perror("Can not pipe\n");
        return ERROR;
    }

    int child1, child2;

    // Порождение процесса-потомка через fork()
    if ((child1 = fork()) == ERR_FORK)
    {
        perror("Can not fork\n");
        return ERROR;
    }
    else if (child1 == OK)
    {
        // printf("\nChild 1: pid = %d, ppid = %d, pgrp = %d\n", getpid(), getppid(), getpgrp());

        close(fd[READ]);
        write(fd[WRITE], TEXT_CHILD_1, strlen(TEXT_CHILD_1) + 1);
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
        // printf("\nChild 2: pid = %d, ppid = %d, pgrp = %d\n\n", getpid(), getppid(), getpgrp());

        close(fd[READ]);
        write(fd[WRITE], TEXT_CHILD_2, strlen(TEXT_CHILD_1) + 1);
        exit(OK);
    }


    if (child1 && child2)
    {
        char text1[TEXT_BUF];
        char text2[TEXT_BUF];

        pid_t child_pid;
        int status;

        close(fd[WRITE]);

        read(fd[READ], text1, TEXT_BUF);
        read(fd[READ], text2, TEXT_BUF);

        // Использование сигналов
        printf(SIGNAL_TEXT);

        sleep(TIME_FOR_SLEEP);

        if (flag == CTRL_SLASH)
        {
            printf("Результат: %s\n\n", text1);
        }
        else if (flag == CTRL_C)
        {
            printf("Результат: %s\n\n", text2);
        }
        else
        {
            printf("\nНичего не выбрано\n\n");
        }

        child_pid = wait(&status); // надо ли?
        check_status(status);

        child_pid = wait(&status);
        check_status(status);
    }

    return OK;
}