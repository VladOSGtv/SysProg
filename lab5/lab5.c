#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <string.h>


int crfLowLevFunIO(char* fileName, size_t fileSize);
int crfDefFunIO(char* fileName, size_t fileSize);
int crfByMmap(char* fileName, size_t fileSize);

int cpyfLowLevFunIO(char* fileName, size_t fileSize);
int cpyDefFunIO(char* fileName, size_t fileSize);
int cpyByMmap(char* fileName, size_t fileSize);

int main(int argc, char** argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <filename> <file_size_in_gb>\n", argv[0]);
        return 1;
    }

    char* fileName = argv[1];
    size_t fileSize = atoi(argv[2]) * 1024 * 1024 * 1024;


  // Створення файлу
    crfLowLevFunIO(fileName, fileSize);
    if (remove(fileName) == -1) 
        perror("Error deleting file");

    crfDefFunIO(fileName, fileSize);
    if (remove(fileName) == -1) 
        perror("Error deleting file");
        
    crfByMmap(fileName, fileSize);


    // Копіювання файлу
    char destFileName[256];
    snprintf(destFileName, sizeof(destFileName), "%s_copy", fileName);

    cpyfLowLevFunIO(fileName, destFileName, fileSize);
    if (remove(destFileName) == -1) 
        perror("Error deleting file");

    cpyDefFunIO(fileName, destFileName, fileSize);
    if (remove(destFileName) == -1) 
        perror("Error deleting file");

    cpyByMmap(fileName, destFileName, fileSize);
    if (remove(destFileName) == -1) 
        perror("Error deleting file");
    
    return 0;
}


int crfLowLevFunIO(char* fileName, size_t fileSize)
{
    char buffer[4096];
    size_t bufSize = sizeof(buffer);
    struct timeval start, end;
    gettimeofday(&start, NULL);

    int fd;
    if ((fd = open(fileName, O_WRONLY | O_CREAT | O_TRUNC)) == -1) {
        perror("Error opening file");
        return 1;
    }

    for (size_t i = 0; i < fileSize; i += bufSize)
        write(fd, buffer, bufSize);
    gettimeofday(&end, NULL);

    printf("Creating file with low-level I/O took %.6f seconds\n", getTimeElapsed(start, end));
    close(fd);
    return 0;
}

int crfDefFunIO(char* fileName, size_t fileSize)
{
    char buffer[4096];
    size_t bufSize = sizeof(buffer);
    struct timeval start, end;
    gettimeofday(&start, NULL);

    FILE* file;
    if ((file = fopen(fileName, "w")) == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < fileSize; i += bufSize)
        fwrite(buffer, 1, bufSize, file);

    // fseek(file, fileSize - 1, SEEK_SET);
    // fputc('\0', file);

    gettimeofday(&end, NULL);
    printf("Creating file with standard file I/O took %.6f seconds\n", getTimeElapsed(start, end));
    fclose(file);
    return 0;
}
int crfByMmap(char* fileName, size_t fileSize)
{
    struct timeval start, end;
    gettimeofday(&start, NULL);

    int fd;

    if((fd = open(fileName, O_RDWR | O_CREAT | O_TRUNC)) == -1){
        perror("Error opening file");
        return 1;
    }

    if (ftruncate(fd, fileSize) == -1) {
        perror("Error resizing file");
        close(fd);
        return 1;
    }

    void * addr;
    if((addr = mmap(0, fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
        perror("Error mapping file");
        close(fd);
        return 1;
    }
    for (size_t i = 0; i < fileSize; i++)
        addr[i] = '1';


    gettimeofday(&end, NULL);
    printf("Creating file with mmap took %.6f seconds\n", getTimeElapsed(start, end));

    if (munmap(addr, fileSize) == -1) {
        perror("Error unmapping file");
    }
    close(fd);
}

int cpyfLowLevFunIO(char* fileName, size_t fileSize)
{
    struct timeval start, end;
    gettimeofday(&start, NULL);



    gettimeofday(&end, NULL);
    printf("Copying file with low-level I/O took %.6f seconds\n", getTimeElapsed(start, end));
    return 0;
}
int cpyDefFunIO(char* fileName, size_t fileSize)
{
    struct timeval start, end;
    gettimeofday(&start, NULL);




    gettimeofday(&end, NULL);
    printf("Copying file with standard file I/O took %.6f seconds\n", getTimeElapsed(start, end));
    return 0;
}
int cpyByMmap(char* fileName, size_t fileSize)
{
    struct timeval start, end;
    gettimeofday(&start, NULL);



    gettimeofday(&end, NULL);
    printf("Copying file with mmap took %.6f seconds\n", getTimeElapsed(start, end));
    return 0;
}
