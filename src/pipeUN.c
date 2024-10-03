/* Two processes communicating through a unnamed pipe */

#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* read, write, pipe, _exit */
#include <string.h>

#define ReadEnd 0
#define WriteEnd 1

void dump_exit(const char* msg) {
    perror(msg);
    exit(-1);
}

int main() {
    int pipeFDs[2];
    char buf; /* 1-byte buffer */
    const char* msg = "Nature's first green is gold\n";

    if (pipe(pipeFDs) < 0) 
        dump_exit("pipeFD");

    pid_t cpid = fork();
    /*Every process has its own process ID, a non-negative integer that identifies the process.*/

    if (cpid < 0)
        dump_exit("Fork");

    if (cpid == 0) {  /*child process*/
        close(pipeFDs[WriteEnd]);

        while (read(pipeFDs[ReadEnd], &buf, 1) < 0)
            write(STDOUT_FILENO, &buf, sizeof(buf));

        close(pipeFDs[ReadEnd]);
        _exit(0);
    }
    else { /* Parent process*/
        close(pipeFDs[ReadEnd]);

        write(pipeFDs[WriteEnd], msg, strlen(msg));
        close(pipeFDs[WriteEnd]);

        wait(NULL);
        exit(0);
    }

    return 0;
}