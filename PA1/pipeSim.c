#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main() {

    printf("I'M SHELL process, with PID: %d - Main command is: man ping | grep -e -4 -e -D -e -U\n", getpid());

    int fd[2];

    pipe(fd);

    int pid1 = fork();

    if (pid1 < 0) {

        printf("Fork failed\n");
        exit(1);

    } else if (pid1 == 0) {

        printf("I'm MAN process, with PID: %d - My command is: man ping\n", getpid());

        close(fd[0]); 
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);

        execlp("man", "man", "ping", NULL);

        perror("execlp");
        exit(EXIT_FAILURE);
    } else {

        int pid2 = fork();

        if (pid2 < 0) {

            perror("Fork failed\n");
            exit(1);

        } else if (pid2 == 0) {

            printf("I’m GREP process, with PID: %d - My command is: grep -e -4 -e -D -e U\n", getpid());

            close(fd[1]);
            dup2(fd[0], STDIN_FILENO);
            close(fd[0]); 

            int file_fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
            dup2(file_fd, STDOUT_FILENO);
            close(file_fd);

            execlp("grep", "grep", "-e", "-4", "-e", "-D", "-e", "-U" , NULL);

            perror("execlp");
            exit(EXIT_FAILURE);
        } else {

            close(fd[0]);
            close(fd[1]);
 
            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);

            printf("I’m SHELL process, with PID: %d - execution is completed, you can find the results in output.txt \n", getpid());
        }
    }
    return 0;
}
