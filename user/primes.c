#include <kernel/types.h>
#include <user/user.h>

#define R 0 // pipe read
#define W 1 // pipe write

void primes(int l_pipe[2]) {
    close(l_pipe[W]);

    int first; // 管道中的第一个数必然是质数
    if (read(l_pipe[R], &first, sizeof(int)) == sizeof(int)) {
        fprintf(1, "prime %d\n", first);

        int r_pipe[2];
        pipe(r_pipe);
        
        // 将左管道的数据向右管道传输，如果左管道的数不能被first整除
        int tmp;
        while (read(l_pipe[R], &tmp, sizeof(int)) == sizeof(int)) {
            if (tmp % first != 0) {
                write(r_pipe[W], &tmp, sizeof(int));
            }
        }
        close(l_pipe[R]);
        close(r_pipe[W]);

        int pid = fork();

        if (pid == 0) {
            primes(r_pipe);
        } else {
            close(r_pipe[R]);
            wait(0);
        }
    }

    exit(0);  
}

int main(int argc, const char *argv[]) {
    int p[2];
    pipe(p);

    for (int i = 2; i <= 35; i++) {
        write(p[W], &i, sizeof(int));
    }
    
    int pid = fork();
    if (pid == 0) {
        primes(p);
    } else if (pid > 0) {
        close(p[W]);
        close(p[R]);
        wait(0);
    } else {
        fprintf(2, "fork() error");
        exit(1);
    }

    exit(0);
}