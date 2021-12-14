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

#include "readwrite.h"
#include "const.h"


int main(void)
{
    setbuf(stdout, NULL);

    int permissions = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    int shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | permissions);

    if (shmid == -1)
    {
        perror("\n\nОшибка: Не удалось создать новый разделяемый сегмент\n\n");

        return -1; // define?
    }

    int *counter;
    counter = (int*)(shmat(shmid, 0, 0));

    if (counter == (void*)-1)
    {
        perror("\n\nОшибка: Не получен указатель на сегмент\n\n");
        return -2;
    }

    // Создать набор семафоров
    int sem_descr = semget(IPC_PRIVATE, 4, IPC_CREAT | permissions);

    if (sem_descr == -1)
    {
        perror("\n\nОшибка: Не удалось создать набор семафоров\n\n");
        return -4;
    }


    if (semctl(sem_descr, READER_ACT, SETVAL, 0) == -1)
    {
        perror("\n\nОшибка: не установлен семафор\n\n");
        return -5;
    }
    if (semctl(sem_descr, WRITER_ACT, SETVAL, 0) == -1)
    {
        perror("\n\nОшибка: не уставновлен семафор\n\n");
        return -6;
    }
    if (semctl(sem_descr, READ_QUEUE, SETVAL, 0) == -1)
    {
        perror("\n\nОшибка: не уставновлен семафор\n\n");
        return -7;
    }
    if (semctl(sem_descr, WRITE_QUEUE, SETVAL, 0) == -1)
    {
        perror("\n\nОшибка: не уставновлен семафор\n\n");
        return -8;
    }

    int child_pid;

    // Для читателей
    for (size_t i = 0; i < READERS_COUNT; i++)
    {
        if ((child_pid = fork()) == -1)
        {
            perror("\n\nОшибка: не удалось прородить процесс читатель\n\n");
            return -9;
        }
        else if (child_pid == 0)
        {
            run_reader(counter, sem_descr, i + 1);
            exit(0);
        }
    }

    // Для писателей
    for (size_t i = 0; i < WRITERS_COUNT; i++)
    {
        if ((child_pid = fork()) == -1)
        {
            perror("\n\nОшибка: не удалось прородить процесс писателя\n\n");
            return -10;
        }
        else if (child_pid == 0)
        {
            run_writer(counter, sem_descr, i + 1);
            exit(0);
        }
    }


    for (size_t i = 0; i < READERS_COUNT + WRITERS_COUNT; i++)
    {
        int status;

        if (wait(&status) == -1)
        {
            perror("\n\nОшибка: wait()\n\n");
            return -11;
        }

        if (!WIFEXITED(status))
        {
            printf("\n\nОшибка: Потомок завершился ненормально\n\n");
        }
    }


    if (shmdt((void*)counter) == -1)
    {
        perror("\n\nОшибка: при попытке отключить разделяемый сегмент от адресного пространства\n\n");
        return -12;
    } 
    if (shmctl(shmid, IPC_RMID, NULL) == -1)
    {
        perror("\n\nОшибка: при попытке пометить сегмент как удаленный\n\n");
        return -13;
    }
    if (semctl(sem_descr, 0, IPC_RMID, 0) == -1)
    {
        perror("\n\nОшибка: при попытке удаления семафора\n\n");
        return -14;
    }

    return 0;
}