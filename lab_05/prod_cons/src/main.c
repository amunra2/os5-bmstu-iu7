#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <wait.h>

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>


#include "buffer_pc.h"
#include "worker.h"


int main(void)
{
    setbuf(stdout, NULL);

    int permissions = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    int shmid = shmget(IPC_PRIVATE, sizeof(buf_t), IPC_CREAT | permissions);

    if (shmid == -1)
    {
        perror("\n\nОшибка: Не удалось создать новый разделяемый сегмент\n\n");

        return -1; // define?
    }

    buf_t *buffer;
    buffer = (buf_t*)(shmat(shmid, 0, 0));

    if (buffer == (void*)-1)
    {
        perror("\n\nОшибка: Не получен указатель на сегмент\n\n");
        return -2;
    }

    if (init_buf(buffer) == -1)
    {
        perror("\n\nОшибка: Не удалось инициалзировать буфер\n\n");
        return -3;
    }

    // Создать набор семафоров
    int sem_descr = semget(IPC_PRIVATE, 3, IPC_CREAT | permissions);

    if (sem_descr == -1)
    {
        perror("\n\nОшибка: Не удалось создать набор семафоров\n\n");
        return -4;
    }

    if (semctl(sem_descr, BINARY_SEM, SETVAL, 1) == -1)
    {
        perror("\n\nОшибка: не установлен бинарный семафор\n\n");
        return -5;
    }
    if (semctl(sem_descr, EMPTY_BUF, SETVAL, N) == -1)
    {
        perror("\n\nОшибка: не уставновлен считающий буфер - БуферПуст\n\n");
        return -6;
    }
    if (semctl(sem_descr, FULL_BUF, SETVAL, 0) == -1)
    {
        perror("\n\nОшибка: не уставновлен считающий буфер - БуферПолон\n\n");
        return -7;
    }

    int child_pid;

    // Для производителей
    for (size_t i = 0; i < PRODUCERS_COUNT; i++)
    {
        if ((child_pid = fork()) == -1)
        {
            perror("\n\nОшибка: не удалось прородить процесс производитель\n\n");
            return -8;
        }
        else if (child_pid == 0)
        {
            run_producer(buffer, sem_descr, i + 1);
            exit(0);
        }
    }

    // Для потребителей
    for (size_t i = 0; i < CONSUMERS_COUNT; i++)
    {
        if ((child_pid = fork()) == -1)
        {
            perror("\n\nОшибка: не удалось прородить процесс потребитель\n\n");
            return -9;
        }
        else if (child_pid == 0)
        {
            run_consumer(buffer, sem_descr, i + 1);
            exit(0);
        }
    }


    for (size_t i = 0; i < PRODUCERS_COUNT + CONSUMERS_COUNT; i++)
    {
        int status;

        if (wait(&status) == -1)
        {
            perror("\n\nОшибка: wait()\n\n");
            return -10;
        }

        if (!WIFEXITED(status))
        {
            printf("\n\nОшибка: Потомок завершился ненормально\n\n");
        }
    }


    if (shmdt((void*)buffer) == -1)
    {
        perror("\n\nОшибка: при попытке отключить разделяемый сегмент от адресного пространства\n\n");
        return -11;
    } 
    if (shmctl(shmid, IPC_RMID, NULL) == -1)
    {
        perror("\n\nОшибка: при попытке пометить сегмент как удаленный\n\n");
        return -12;
    }
    if (semctl(sem_descr, 0, IPC_RMID, 0) == -1)
    {
        perror("\n\nОшибка: при попытке удаления семафора\n\n");
        return -13;
    }

    return 0;
}