#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/fcntl.h>


int create_process()
{
    pid_t pid = fork();
    if (-1 == pid)
    {
        write(STDERR_FILENO, "fork", 5);
        exit(-1);
    }
    return pid;
}

int main(int argc, char *argv[])
{

    int fd[2]; // parent -> child
    if (pipe(fd) == -1)
    {
        write(STDERR_FILENO, "Pipe wasn\'t created", 20);
    }
    int fd2[2]; // child -> parent
    if (pipe(fd2) == -1)
    {
        write(STDERR_FILENO, "Pipe wasn\'t created", 20);
    }

    pid_t pid = create_process();

    if (0 == pid) // Child
    {
        close(fd[1]);
        dup2(fd[0], STDIN_FILENO);

        close(fd2[0]);
        dup2(fd2[1], STDOUT_FILENO);

        execvp("./child", argv);
    }
    else // Parent
    {
        // Enter file path
        char file_name[128];
        read(STDIN_FILENO,file_name, 128);
        int pos = strlen(file_name) - 1;
        if (file_name[pos] == '\n')
        {
            file_name[pos] = '\0';
        }

        // Open the file
        int file = open(file_name, O_RDONLY);
        char numbers[128];
        if (file != -1)
        {
            read(file, numbers, 128);
            write(STDERR_FILENO, "The file successfully opened\n", 30);
        }
        else
        {
            write(STDERR_FILENO, "The file did\'t open\n", 21);
        }

        // Send 'numbers' to the child
        close(fd[0]);
        write(fd[1], numbers, 128 /* sizeof(numbers) */);
        //printf(_G_ "Parent send '%s'\n", numbers);
        close(fd[1]);

        waitpid(pid, NULL, WUNTRACED);

        // Read 'answer' from the child
        close(fd2[1]);
        char answer[128];
        read(fd2[0], answer, 128 /* sizeof(answer) */);
        write(STDOUT_FILENO, answer, 128);

        close(fd2[0]);
    }
    //printf(RESET_COLOR);
    return 0;
}