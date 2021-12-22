#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define ITERS 8
#define WRITE_TIMEOUT 300
#define READ_TIMEOUT 300
#define DIFF 4000

HANDLE mutex;
HANDLE can_read;
HANDLE can_write;

int waiting_writers = 0;
int waiting_readers = 0;
int active_readers = 0;
bool active_writer = false;

int value = 0;

void start_read(void) 
{
    InterlockedIncrement(&waiting_readers);

    if (active_writer || (WaitForSingleObject(can_write, 0) == WAIT_OBJECT_0 && waiting_writers))
    {
        WaitForSingleObject(can_read, INFINITE);
    }

    WaitForSingleObject(mutex, INFINITE);

    InterlockedDecrement(&waiting_readers);
    InterlockedIncrement(&active_readers);

    SetEvent(can_read);
    ReleaseMutex(mutex);
}

void stop_read(void) 
{
    InterlockedDecrement(&active_readers);

    if (active_readers == 0) 
    {
        ResetEvent(can_read);
        SetEvent(can_write);
    }
}

DWORD WINAPI run_reader(CONST LPVOID parametr) 
{
    int index = (int)parametr;
    int sleep_time;
    srand(time(NULL) + index);

    for (size_t i = 0; i < ITERS; i++) 
    {
        sleep_time = READ_TIMEOUT + rand() % DIFF;
        Sleep(sleep_time);

        start_read();

        printf("\x1b[32m Читатель %ld прочитал:  %5ld\x1b[0m (состояние сна %4d мс.)\n", index + 1, value, sleep_time);

        stop_read();
    }

    return 0;
}


void start_write(void) 
{
    InterlockedIncrement(&waiting_writers);

    if (active_writer || active_readers > 0)
    { 
        WaitForSingleObject(can_write, INFINITE);
    }
    
    InterlockedDecrement(&waiting_writers);
    active_writer = true;
}

void stop_write(void) 
{
    active_writer = false;

    if (waiting_readers)
    {
        SetEvent(can_read);
    }
    else 
    {
        SetEvent(can_write);
    }
}

DWORD WINAPI run_writer(CONST LPVOID parametr) 
{
    int index = (int)parametr;
    int sleep_time;
    srand(time(NULL) + index + 5);

    for (int i = 0; i < ITERS; i++) 
    {
        sleep_time = WRITE_TIMEOUT + rand() % DIFF;
        Sleep(sleep_time);

        start_write();

        value++;
        printf("\x1b[33m Писатель %ld записал:   %5ld\x1b[0m (состояние сна %4d мс.)\n", index + 1, value, sleep_time);

        stop_write();
    }

    return 0;
}


int main(void) 
{
    setbuf(stdout, NULL);

    HANDLE readers_threads[5];
    HANDLE writers_threads[3];

    if ((mutex = CreateMutex(NULL, FALSE, NULL)) == NULL)
    {
        perror("\nОшибка при создании мьютекса.\n");

        return 1;
    }

    if ((can_read = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL)
    {
        perror("\nОшибка при создании события.\n");

        return 2;
    }
    
    if ((can_write = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL) 
    {
        perror("\nОшибка при создании события.\n");

        return 3;
    }


    for (int i = 0; i < 5; i++)
    {
        if ((readers_threads[i] = CreateThread(NULL, 0, run_reader, (LPVOID)i, 0, NULL)) == NULL)
        {
            perror("\nОшибка при создании процесса читателя\n");

            return 4;
        }
    }

    for (int i = 0; i < 3; i++)
    {
        if ((writers_threads[i] = CreateThread(NULL, 0, run_writer, (LPVOID)i, 0, NULL)) == NULL) 
        {
            perror("\nОшибка при создании процесса писателя\n");

            return 5;
        }
    }

    WaitForMultipleObjects(5, readers_threads, TRUE, INFINITE);
    WaitForMultipleObjects(3, writers_threads, TRUE, INFINITE);

    CloseHandle(mutex);
    CloseHandle(can_read);
    CloseHandle(can_write);

    return 0;
}
