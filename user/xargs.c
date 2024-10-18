#include <kernel/types.h>
#include <kernel/param.h>
#include <user/user.h>

enum state {
    S_ARG,
    S_ARG_END,
    S_ENDL,
    S_ARG_ENDL,
    S_END,
    S_WAIT
};

enum char_type {
    C_ENDL,
    C_CHAR,
    C_SPACE
};

enum char_type get_char_type(char ch) {
    switch (ch) {
    case '\n':
        return C_ENDL;
    case ' ':
        return C_SPACE;
    default:
        return C_CHAR;
    }
}

enum state trans(enum state cur, char next) {
    enum char_type ct = get_char_type(next);
    switch (cur) {
    case S_ARG:
        if (ct == C_CHAR) return S_ARG;
        if (ct == C_ENDL) return S_ARG_ENDL;
        if (ct == C_SPACE) return S_ARG_END;
        break;
    case S_END:
        return S_END;
    default:
        if (ct == C_SPACE) return S_WAIT;
        if (ct == C_ENDL) return S_ENDL;
        if (ct == C_CHAR) return S_ARG;
        break;
    }
}

void clearArgv(char *x_argv[MAXARG], int idx_start) {
    for (int i = idx_start; i < MAXARG; i++) {
        x_argv[i] = 0;
    }
}

int main(int argc, const char* argv[]) {
    if (argc - 1 >= MAXARG) {
        fprintf(2, "too many args\n");
        exit(1);
    }
    
    char line[512];
    char *p = line;  
    char *x_argv[MAXARG] = {0};

    for (int i = 1; i < argc; i++) {
        x_argv[i - 1] = argv[i];
    }

    enum state st = S_WAIT;
    int arg_start = 0;
    int arg_end = 0;
    int arg_cnt = argc - 1;

    while (st != S_END) {
        if (read(0, p, sizeof(char)) <= 0) {
            st = S_END;
        } else {
            st = trans(st, *p);
        }

        arg_end++;
        if (arg_end >= 512) {
            fprintf(2, "xargs: argument line too long\n");
            exit(1);
        }

        switch (st) {
            case S_WAIT:
                arg_start++;
                break;
            case S_ARG_END:
                *p = '\0'; 
                x_argv[arg_cnt] = line + arg_start;
                arg_cnt++;
                arg_start = arg_end;
                break;
            case S_ARG_ENDL:
                x_argv[arg_cnt] = line + arg_start;
                arg_cnt++;
            case S_ENDL:
                *p = '\0';  
                arg_start = arg_end;
                if (fork() == 0) {
                    exec(argv[1], x_argv);
                }
                arg_cnt = argc - 1;
                clearArgv(x_argv, arg_cnt);
                wait(0);
                break;
            default:
                break;
        }

        p++;
    }

    exit(0);
}
