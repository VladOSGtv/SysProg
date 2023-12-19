#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/time.h>

double getTimeElapsed(struct timeval start, struct timeval end);
void copyFile(char* source, char* dest);
int main(int argc, char* argv) {
    char* source_filename = "source.txt";
    char* dest_filename = "dest.txt";

    // Method #1
    pid_t child_pid;
    struct timeval start, end;

    gettimeofday(&start, NULL);
        child_pid = fork();
    if (child_pid == 0) {
        copyFile(source_filename, dest_filename);
        exit(0);
    } else if (child_pid > 0) {
        wait(NULL);
         gettimeofday(&end, NULL);
        printf("Runtime of fork(): %f sec\n", getTimeElapsed(start, end));
    } else {
        printf("Error calling fork()\n");
        return 1;
    }

    // Method #2
    gettimeofday(&start, NULL);
    pid_t exec_child_pid = fork();
    if (exec_child_pid == 0) {
        char* const args[] = {"cp", source_filename, dest_filename, NULL};
        execv("/bin/cp", args);
        printf("exec() execution error \n");
        exit(1);
    } else if (exec_child_pid > 0) {
        wait(NULL);
         gettimeofday(&end, NULL);
        printf("Runtime of exec(): %f sec\n", getTimeElapsed(start, end));
    } else {
        printf("Error calling fork()\n");
        return 1;
    }

    // Method #3
 gettimeofday(&start, NULL);
    int status = system("cp source.txt dest.txt");
    if (status == -1) {
        printf("system() execution error\n");
        return 1;
    }
     gettimeofday(&end, NULL);
    printf("Runtime of system(): %f sec\n", getTimeElapsed(start, end));

    return 0;
}

double getTimeElapsed(struct timeval start, struct timeval end) {return (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_usec - start.tv_usec)/1000000;}

void copyFile(char* source, char* dest) {
    FILE *src, *dest;
    char ch;

    src = fopen(source, "r");
    if (src == NULL) {
        printf("Error opening source file.\n");
        exit(1);
    }

    dest = fopen(dest, "w");
    if (dest == NULL) {
        fclose(src);
        printf("Error opening dest file.\n");
        exit(1);
    }

    while ((ch = fgetc(src)) != EOF) {
        fputc(ch, dest);
    }

    fclose(src);
    fclose(dest);
    printf("File copied successfully.\n");
}
