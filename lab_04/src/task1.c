#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#define TIME_FOR_SLEEP 3

#define TASK_TEXT "\n=======================\
                    \n     Task 1: fork()    \
                    \n=======================\n\n"


int main()
{
    printf(TASK_TEXT);
    
    int child1, child2;

    // Порождение процесса-потомка через fork()
    if ((child1 = fork()) == -1)
    {
        perror("Can not fork\n");
        return -1;
    }
    else if (child1 == 0)
    {   
        printf("\nBefore sleep: Child 1: pid = %d, ppid = %d, pgrp = %d\n", getpid(), getppid(), getpgrp());

        sleep(TIME_FOR_SLEEP);

        printf("\nAfter sleep: Child 1: pid = %d, ppid = %d, pgrp = %d\n", getpid(), getppid(), getpgrp());

        exit(0);
    }


    if ((child2 = fork()) == -1)
    {
        perror("Can not fork\n");
        return -1;
    }
    else if (child2 == 0)
    {
        printf("\nBefore sleep: Child 2: pid = %d, ppid = %d, pgrp = %d\n", getpid(), getppid(), getpgrp());

        sleep(TIME_FOR_SLEEP);

        printf("\nAfter sleep: Child 2: pid = %d, ppid = %d, pgrp = %d\n", getpid(), getppid(), getpgrp());

        exit(0);
    }


    printf("\n\nParent: pid = %d, pgrp = %d\n", getpid(), getpgrp());

    return 0;
}
