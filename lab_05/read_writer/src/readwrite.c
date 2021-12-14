#include "readwrite.h"


struct sembuf reader_begin[5] = 
{
    {READ_QUEUE, 1, 0},
    {WRITER_ACT, 0, 0},
    {WRITE_QUEUE, 0, 0},
    {READER_ACT, 1, 0},
    {READ_QUEUE, -1, 0}
};

struct sembuf reader_release[] = {{READER_ACT, -1, 0}};


struct sembuf writer_begin[5] = 
{
    {WRITE_QUEUE, 1, 0},
    {READER_ACT, 0, 0},
    {WRITER_ACT, 0, 0},
    {WRITER_ACT, 1, 0},
    {WRITE_QUEUE, -1, 0}
};

struct sembuf writer_release[] = {{WRITER_ACT, -1, 0}};


int run_reader(int *const counter, const int sid, const int reader_id)
{
    if (!counter)
    {
        return -1;
    }

    srand(time(NULL));

    int sleep_time;
    int elem;

    for (int i = 0; i < ITERS; i++)
    {
        sleep_time = rand() % 4 + 1;
        sleep(sleep_time);

        if (semop(sid, reader_begin, 5) == -1)
        {
            perror("\n\nОшибка: Читатель не может изменить значение семафора\n\n");
            exit(-1);
        }

        // Начало критической секции
        elem = *counter;
        // Конец критической секции

        printf("\e[1;34mЧитатель #%d читает: %4d\e[0m (в состоянии сна %d с.)\n", reader_id, elem, sleep_time);
        

        // Читатель завершает работу
        if (semop(sid, reader_release, 1) == -1)
        {
            perror("\n\nОшибка: Читатель не может изменить значение семафора\n\n");
            exit(-2);
        }
    }

    return 0;
}


int run_writer(int *const counter, const int sid, const int writer_id)
{
    if (!counter)
    {
        return -1;
    }

    srand(time(NULL));

    int sleep_time;
    char elem;

    for (int i = 0; i < ITERS; i++)
    {
        sleep_time = rand() % 3 + 1;
        sleep(sleep_time);

        if (semop(sid, writer_begin, 5) == -1)
        {
            perror("\n\nОшибка: Писатель не может изменить значение семафора\n\n");
            exit(-3);
        }

        // Начало критической секции
        elem = ++(*counter);
        // Конец критической секции

        printf("\e[1;33mПисатель #%d пишет:  %4d\e[0m (в состоянии сна %d с.)\n", writer_id, elem, sleep_time);
        
        // Писатель завершает работу
        if (semop(sid, writer_release, 1) == -1)
        {
            perror("\n\nОшибка: Писатель не может изменить значение семафора\n\n");
            exit(-4);
        }
    }

    return 0;
}
