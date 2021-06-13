#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>

//grep -v usr < /etc/passwd | wc -l > r.txt; cat r.txt
//result: 5

int main()
{
    int pipefd[2], pid, status;
    pipe(pipefd); //new pipeline
    if ((pid = fork()) == -1)
    {
        perror("fork");
        exit(1);
    }
    else if (pid == 0)
    { //parent open file
        int fd0 = open("/etc/passwd", O_RDONLY);
        if (fd0 != -1)
        {                            //get filefd success
            fflush(stdout);          //flush buffer
            dup2(fd0, STDIN_FILENO); //redirect STDIN to fd0
            close(fd0);
        }
        dup2(pipefd[1], STDOUT_FILENO); //redirect STDOUT to pipefd1
        close(pipefd[0]);
        close(pipefd[1]);
        execlp("grep", "grep", "-v", "usr", NULL); //run command, input from fd0, output to pipefd1
        exit(1);
    }
    if ((pid = fork()) == -1)
    {
        perror("fork");
        exit(1);
    }
    else if (pid == 0)
    {
        //create r.txt if not exist, use -rw-rw-rw-
        int fd1 = open("r.txt", O_CREAT | O_WRONLY, 0666);
        if (fd1 != -1)
        {                             //get filefd success
            dup2(fd1, STDOUT_FILENO); //redirect STDOUT to fd1
            close(fd1);
        }
        dup2(pipefd[0], STDIN_FILENO); //redirect STDIN to pipefd0
        close(pipefd[0]);
        close(pipefd[1]);
        execlp("wc", "wc", "-l", NULL); //run command, input from pipefd0, output to fd1
        exit(1);
    }
    close(pipefd[0]);
    close(pipefd[1]);
    wait(&status);
    wait(&status);                       //wait for 2 children
    execlp("cat", "cat", "r.txt", NULL); //print to STDOUT
    return 0;
}
