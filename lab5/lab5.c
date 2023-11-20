#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <string.h>


void crfLowLevFunIO(char* fileName, size_t fileSize);
void crfDefFunIO(char* fileName, size_t fileSize);
void crfByMmap(char* fileName, size_t fileSize);

void cpyfLowLevFunIO(char* fileName, size_t fileSize);
void cpyDefFunIO(char* fileName, size_t fileSize);
void cpyByMmap(char* fileName, size_t fileSize);

int main(int argc, char** argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <filename> <file_size_in_gb>\n", argv[0]);
        return 1;
    }

    char* fileName = argv[1];
    size_t fileSizeGB = atoi(argv[2]);
    size_t fileSize = fileSizeGB * 1024 * 1024 * 1024;




  // Створення файлу
    struct timeval start, end;
    gettimeofday(&start, NULL);
    crfLowLevFunIO(fileName, fileSize);
    gettimeofday(&end, NULL);
    printf("Creating file with low-level I/O took %.6f seconds\n", getTimeElapsed(start, end));

 // Видалення файлу
    if (remove(fileName) == -1) 
        perror("Error deleting file");

    gettimeofday(&start, NULL);
    crfDefFunIO(fileName, fileSize);
    gettimeofday(&end, NULL);
    printf("Creating file with standard file I/O took %.6f seconds\n", getTimeElapsed(start, end));
 
 // Видалення файлу
    if (remove(fileName) == -1) 
        perror("Error deleting file");
        
    gettimeofday(&start, NULL);
    crfByMmap(fileName, fileSize);
    gettimeofday(&end, NULL);
    printf("Creating file with mmap took %.6f seconds\n", getTimeElapsed(start, end));


    // Копіювання файлу
    char destFileName[256];
    snprintf(destFileName, sizeof(destFileName), "%s_copy", fileName);

    gettimeofday(&start, NULL);
    cpyfLowLevFunIO(fileName, destFileName, fileSize);
    gettimeofday(&end, NULL);
    printf("Copying file with low-level I/O took %.6f seconds\n", getTimeElapsed(start, end));

 // Видалення файлу
    if (remove(destFileName) == -1) 
        perror("Error deleting file");

    gettimeofday(&start, NULL);
    cpyDefFunIO(fileName, destFileName, fileSize);
    gettimeofday(&end, NULL);
    printf("Copying file with standard file I/O took %.6f seconds\n", getTimeElapsed(start, end));

 // Видалення файлу
    if (remove(destFileName) == -1) 
        perror("Error deleting file");

    gettimeofday(&start, NULL);
    cpyByMmap(fileName, destFileName, fileSize);
    gettimeofday(&end, NULL);
    printf("Copying file with mmap took %.6f seconds\n", getTimeElapsed(start, end));

 // Видалення файлу
    if (remove(destFileName) == -1) 
        perror("Error deleting file");
    
    return 0;
}


void crfLowLevFunIO(char* fileName, size_t fileSize)
{
    int fd = open(fileName, O_WRONLY | O_CREAT | O_TRUNC);
    if (fd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd, fileSize) == -1) {
        perror("Error resizing file");
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd);
}

void crfDefFunIO(char* fileName, size_t fileSize)
{
    FILE* file = fopen(fileName, "wb");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    fseek(file, fileSize - 1, SEEK_SET);
    fputc('\0', file);

    fclose(file);
}
void crfByMmap(char* fileName, size_t fileSize);

void cpyfLowLevFunIO(char* fileName, size_t fileSize);
void cpyDefFunIO(char* fileName, size_t fileSize);
void cpyByMmap(char* fileName, size_t fileSize);
