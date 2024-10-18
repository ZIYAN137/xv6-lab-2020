#include "kernel/types.h"
#include "user/user.h"

#define R 0 // pipe read
#define W 1 // pipe write

int main(int argc, char const *argv[])
{
    int fd_p2c[2];
    int fd_c2p[2];
    char buffer = 'P';

    pipe(fd_c2p);
    pipe(fd_p2c);

    int pid = fork(); 
    int exit_status = 0;

    if (pid < 0) { 
        fprintf(2, "fork() error!\n");
        close(fd_c2p[R]);
        close(fd_c2p[W]);
        close(fd_p2c[R]);
        close(fd_p2c[W]);
        exit(1);
    } else if (pid == 0) { // child
        close(fd_c2p[R]);
        close(fd_p2c[W]);

        if (read(fd_p2c[R], &buffer, sizeof(char)) != sizeof(char)) {
            fprintf(2, "child read() error!\n");
            exit_status = 1;
        } else {
            fprintf(1, "%d: received ping\n", getpid());
        }

        if (write(fd_c2p[W], &buffer, sizeof(char)) != sizeof(char)) {
            fprintf(2, "child write() error!\n");
            exit_status = 1;
        }

        close(fd_c2p[W]);
        close(fd_p2c[R]);
        exit(exit_status);
    } else { // parent
        close(fd_p2c[R]);
        close(fd_c2p[W]);

        if (write(fd_p2c[W], &buffer, sizeof(char)) != sizeof(char)) {
            fprintf(2, "parent write() error!\n");
            exit_status = 1;
        }

        if (read(fd_c2p[R], &buffer, sizeof(char)) != sizeof(char)) {
            fprintf(2, "parent read() error!\n");
            exit_status = 1;
        } else {
            fprintf(1, "%d: received pong\n", getpid());
        }

        close(fd_p2c[W]);
        close(fd_c2p[R]);
        exit(exit_status);
        
    }
}