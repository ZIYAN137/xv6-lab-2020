#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(const char* directory, const char* filename) {
    char buffer[1024], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(directory, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", directory);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: connot fstat %s\n", directory);
        return;
    }

    if (st.type != T_DIR) {
        fprintf(2, "usage: find <directory> <filename>\n");
        exit(1);
    }

    if (strlen(directory) + 1 + DIRSIZ + 1 > sizeof(buffer)) {
        fprintf(2, "directory is too long\n");
        exit(1);
    }

    strcpy(buffer, directory);
    p = buffer + strlen(buffer);
    *p = '/';
    p++;

    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0) {
            continue;
        } 
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if (stat(buffer, &st) < 0) {
            fprintf(2, "find: connot stat %s\n", buffer);
            continue;
        }
        if (st.type == T_DIR && strcmp(p, ".") != 0 && strcmp(p, "..") != 0) {
            find(buffer, filename);
        } else if (strcmp(p, filename) == 0) {
            fprintf(1, "%s\n", buffer);
        }
    }

    close(fd);
    return;
}

int main(int argc, const char* argv[]) {
    if (argc != 3) {
        fprintf(2, "usage: find <directory> <filename>\n");
        exit(1);
    }

    find(argv[1], argv[2]);
    
    exit(0);
}