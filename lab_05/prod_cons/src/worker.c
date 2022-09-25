#include "worker.h"


struct sembuf producer_lock[2] = {{EMPTY_BUF, -1, 0}, {BINARY_SEM, -1, 0}};
struct sembuf producer_release[2] = {{BINARY_SEM, 1, 0}, {FULL_BUF, 1, 0}};

struct sembuf consumer_lock[2] = {{FULL_BUF, -1, 0}, {BINARY_SEM, -1, 0}};
struct sembuf consumer_release[2] = {{BINARY_SEM, 1, 0}, {EMPTY_BUF, 1, 0}};



int run_producer(buf_t *const buffer, const int sid, const int producer_id)
{
    if (!buffer)
    {
        return -1;
    }

    srand(time(NULL));

    int sleep_time;
    char elem;

    for (int i = 0; i < ITERS; i++)
    {
        sleep_time = rand() % 2 + 1;
        sleep(sleep_time);


        // Происходит:
        // buf_empty -= 1 (если buf_empty = 0 - блокируется производитель, происходит ожидание освобождения ячейки)
        // bin_sem -= 1 (ожидание выхода из критической секции)
        if (semop(sid, producer_lock, 2) == -1)
        {
            perror("\n\nОшибка: Производитель не может изменить значение семафора\n\n");
            exit(-1);
        }

        // Начало критической секции
        elem = 'a' + (char)(buffer->w_pos % LETTERS_ALL);

        if (write_buf(buffer, elem) == -1)
        {
            perror("\n\nОшибка: при записи в буфер\n\n");
            return -2;
        }
        // Конец критической секции

        printf("\e[1;34mПроизводитель #%d записал: %4c\e[0m (в состоянии сна %d с.)\n", producer_id, elem, sleep_time);
        


        // Производитель выполнил задачу
        // Происходит:
        // buf_full += 1 (инкремент кол-ва заполненных ячеек)
        // bin_sem += 1 (освобождение критической секции)
        if (semop(sid, producer_release, 2) == -1)
        {
            perror("\n\nОшибка: Производитель не может изменить значение семафора\n\n");
            exit(-2);
        }
    }

    return 0;
}


int run_consumer(buf_t *const buffer, const int sid, const int consumer_id)
{
    if (!buffer)
    {
        return -1;
    }

    srand(time(NULL));

    int sleep_time;
    char elem;

    for (int i = 0; i < ITERS; i++)
    {
        sleep_time = rand() % 7 + 1;
        sleep(sleep_time);


        // Происходит:
        // buf_full -= 1 (если buf_full = 0 - блокируется потребитель, происходит ожидание заполнения ячейки)
        // bin_sem -= 1 (ожидание выхода из критической секции)
        if (semop(sid, consumer_lock, 2) == -1)
        {
            perror("\n\nОшибка: Потребитель не может изменить значение семафора\n\n");
            exit(-3);
        }

        // Начало критической секции
        if (read_buf(buffer, &elem) == -1)
        {
            perror("\n\nОшибка: при чтении из буфера\n\n");
            return -3;
        }

        printf("\e[1;33mПотребитель #%d прочитал:  %4c\e[0m (в состоянии сна %d с.)\n", consumer_id, elem, sleep_time);
        // Конец критической секции


        // Потребитель выполнил задачу
        // Происходит:
        // buf_empty += 1 (инкремент кол-ва пустых ячеек)
        // bin_sem += 1 (освобождение критической секции)
        if (semop(sid, consumer_release, 2) == -1)
        {
            perror("\n\nОшибка: Потребитель не может изменить значение семафора\n\n");
            exit(-4);
        }
    }

    return 0;
}
