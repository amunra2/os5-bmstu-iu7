#include <stdio.h>

#define SIZE 10


void qsort(int *s_arr, int first, int last)
{
    if (first < last)
    {
        int left = first, right = last, middle = s_arr[(left + right) / 2];

        do
        {
            while (s_arr[left] < middle) left++;
            while (s_arr[right] > middle) right--;

            if (left <= right)
            {
                int tmp = s_arr[left];
                s_arr[left] = s_arr[right];
                s_arr[right] = tmp;
                left++;
                right--;
            }
        } while (left <= right);

        qsort(s_arr, first, right);
        qsort(s_arr, left, last);
    }
}


int main(void)
{
    int arr[SIZE], arr_size;

    printf("\nПрограмма 2: Сортировка массива методом QuickSort");

    printf("\n\nВведите размер массива: ");

    if (scanf("%d", &arr_size) == 0)
    {
        printf("\n\nНеправильно введен размер массива");
        return -2;
    }

    printf("\n\nИзначальный массив: ");

    for (int i = 0; i < arr_size; i++)
    {
        if (scanf("%d", &arr[i]) == 0)
        {
            printf("\n\nНеправильно введен элемент массива");
            return -3;
        }
    }

    qsort(arr, 0, arr_size - 1);

    printf("\n\nОтсортированный массив: ");

    for (int i = 0; i < arr_size; i++)
    {
        printf("%d ", arr[i]);
    }

    printf("\n\n");

    return 0;
}