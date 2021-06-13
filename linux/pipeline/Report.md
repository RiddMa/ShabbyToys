<p align="center">《Linux开发环境及应用》</p>

<h1 align="center">第四次上机作业</h2>

<p align = 'right'>姓名：马嘉骥，学号：2018211149</p>
<p align = 'right'>班级：2018211303，班内序号：15</p>

## 题目

使用 fork(), exec(), dup2(), pipe(), open(), wait() 等系统调用编写C语言程序完成与下列shell命令等价的功能。

`grep -v usr < /etc/passwd | wc -l > r.txt; cat r.txt `

（提示：为简化编程，不需要用strtok断词，直接用execlp实现能达到shell命令相同功能的程序即可），例如：execlp("grep", "grep", "-v", "usr", 0);

## 源代码

```C
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
    execlp("cat", "cat", "r.txt", NULL); //cat to STDOUT
    return 0;
}
```

## 运行结果

```shell
root@RIDD-DESKTOP:/mnt/c/Users/Ridd/GitRepos/ShabbyToys/linux/pipeline# grep -v usr < /etc/passwd | wc -l > r.txt; cat r.txt
5
root@RIDD-DESKTOP:/mnt/c/Users/Ridd/GitRepos/ShabbyToys/linux/pipeline# gcc pipe.c -o pipe
root@RIDD-DESKTOP:/mnt/c/Users/Ridd/GitRepos/ShabbyToys/linux/pipeline# ./pipe
5
root@RIDD-DESKTOP:/mnt/c/Users/Ridd/GitRepos/ShabbyToys/linux/pipeline# 
```

## 总结

本C程序使用了 fork(), exec(), dup2(), pipe(), open(), wait() 等系统调用，创建管道在子进程间通信，使用dup2重定向标准输入输出，实现了`grep -v usr < /etc/passwd | wc -l > r.txt; cat r.txt`等价的功能。

