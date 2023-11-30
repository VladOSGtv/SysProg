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

int cpyfLowLevFunIO(char* fileName, char* destFileName);
int cpyDefFunIO(char* fileName, char* destFileName);
int cpyByMmap(char* fileName, char* destFileName);

double getTimeElapsed(struct timeval start, struct timeval end);

int main(int argc, char** argv)
{
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <filename> <file_size_in_gb> \n", argv[0]);
        return 1;
    }

    const char *operation = argv[1];

    if (strcmp(operation, "create_lowlevel") == 0)
    {
        if (argc != 4)
        {
            printf("Потрібно вказати ім'я файлу та розмір для операції 'create_lowlevel'.\n");
            return 1;
        }
        const char *filename = argv[2];
        off_t file_size = atoll(argv[3]);
        crfLowLevFunIO(filename, file_size);
    }
    else if (strcmp(operation, "create_standard") == 0)
    {
        if (argc != 4)
        {
            printf("Потрібно вказати ім'я файлу та розмір для операції 'create_standard'.\n");
            return 1;
        }
        const char *filename = argv[2];
        off_t file_size = atoll(argv[3]);
        crfDefFunIO(filename, file_size);
    }
    else if (strcmp(operation, "create_mmap") == 0)
    {
        if (argc != 4)
        {
            printf("Потрібно вказати ім'я файлу та розмір для операції 'create_mmap'.\n");
            return 1;
        }
        const char *filename = argv[2];
        off_t file_size = atoll(argv[3]);
        crfByMmap(filename, file_size);
    }
    else if (strcmp(operation, "copy_lowlevel") == 0)
    {
        if (argc != 4)
        {
            printf("Потрібно вказати ім'я вихідного та нового файлів для операції 'copy_lowlevel'.\n");
            return 1;
        }
        const char *source_filename = argv[2];
        const char *dest_filename = argv[3];
        cpyfLowLevFunIO(source_filename, dest_filename);
    }
    else if (strcmp(operation, "copy_standard") == 0)
    {
        if (argc != 4)
        {
            printf("Потрібно вказати ім'я вихідного та нового файлів для операції 'copy_standard'.\n");
            return 1;
        }
        const char *source_filename = argv[2];
        const char *dest_filename = argv[3];
        cpyDefFunIO(source_filename, dest_filename);
    }
    else if (strcmp(operation, "copy_mmap") == 0)
    {
        if (argc != 4)
        {
            printf("Потрібно вказати ім'я вихідного та нового файлів для операції 'copy_mmap'.\n");
            return 1;
        }
        const char *source_filename = argv[2];
        const char *dest_filename = argv[3];
        cpyByMmap(source_filename, dest_filename);
    }
    else
    {
        printf("Невідома операція: %s\n", operation);
        return 1;
    }

    return 0;
}


int crfLowLevFunIO(char* fileName, size_t fileSize)
{
    char buffer[4096];
    size_t bufSize = sizeof(buffer);
    struct timeval start, end;
    gettimeofday(&start, NULL);


    int fd;
    if ((fd = open(fileName, O_WRONLY | O_CREAT, 0644)) == -1) {
        perror("Error opening file");
        return 1;
    }

    for (size_t i = 0; i < fileSize; i += bufSize)
        write(fd, buffer, bufSize);
    gettimeofday(&end, NULL);

    printf("\tCreating file with low-level I/O took %.10f seconds\n", getTimeElapsed(start, end));
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
    printf("\tCreating file with standard file I/O took %.6f seconds\n", getTimeElapsed(start, end));
    fclose(file);
    return 0;
}
int crfByMmap(char* fileName, size_t fileSize)
{
    struct timeval start, end;
    gettimeofday(&start, NULL);

    int fd;

    if((fd = open(fileName, O_RDWR | O_CREAT, 0644)) == -1){
        perror("Error opening file");
        return 1;
    }

    if (ftruncate(fd, fileSize) == -1) {
        perror("Error resizing file");
        close(fd);
        return 1;
    }

    unsigned char * addr;
    if((addr = (unsigned char*) mmap(0, fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
        perror("Error mapping file");
        close(fd);
        return 1;
    }
    for (size_t i = 0; i < fileSize; i++)
        addr[i] = '1';


    gettimeofday(&end, NULL);
    printf("\tCreating file with mmap took %.6f seconds\n", getTimeElapsed(start, end));

    if (munmap(addr, fileSize) == -1) {
        perror("Error unmapping file");
    }
    close(fd);
}

int cpyfLowLevFunIO(char* fileName, char* destFileName)
{
    struct timeval start, end;
    gettimeofday(&start, NULL);

    int fileRd;
    if ((fileRd = open(fileName, O_RDONLY | O_CREAT, 0644)) == -1) {
        perror("Error opening file");
        return 1;
    }
    int fileWr;
    if ((fileWr = open(fileName, O_WRONLY | O_CREAT, 0644)) == -1) {
        perror("Error opening file");
        return 1;
    }

    char buffer[4096];
    ssize_t bytesRead, bytesWritten, bufSize = sizeof(buffer);

    while(bytesRead = read(fileRd, buffer, bufSize) > 0){
        bytesWritten = write(fileWr, buffer, bytesRead);
         if (bytesWritten != bytesRead) {
            perror("Error writing to destination file");
            close(fileRd);
            close(fileWr);
            return 1;
        }
    }

    gettimeofday(&end, NULL);

    if (bytesRead == -1) {
        perror("Error reading from source file");
        close(fileRd);
        close(fileWr);
        return 1;
    }

    printf("\tCopying file with low-level I/O took %.6f seconds\n", getTimeElapsed(start, end));
    close(fileRd);
    close(fileWr);
    return 0;
}
int cpyDefFunIO(char* fileName, char* destFileName)
{
    struct timeval start, end;
    gettimeofday(&start, NULL);

    FILE* fileRd;
    if ((fileRd = fopen(fileName, "r")) == -1) {
        perror("Error opening file");
        return 1;
    }
    FILE* fileWr;
    if ((fileWr = fopen(fileName, "w")) == -1) {
        perror("Error opening file");
        return 1;
    }

    char buffer[4096];
    ssize_t bytesRead, bytesWritten, bufSize = sizeof(buffer);

    while(bytesRead = fread(buffer, 1, bufSize, fileRd) > 0){
        bytesWritten = fwrite(buffer, 1, bytesRead, fileWr);
        if (bytesWritten != bytesRead) {
            perror("Error writing to destination file");
            close(fileRd);
            close(fileWr);
            return 1;
        }
    }

    gettimeofday(&end, NULL);

    if (bytesRead == -1) {
        perror("Error reading from source file");
        close(fileRd);
        close(fileWr);
        return 1;
    }

    printf("\tCopying file with standard file I/O took %.6f seconds\n", getTimeElapsed(start, end));
    return 0;
}
int cpyByMmap(char* fileName, char* destFileName)
{
    struct timeval start, end;
    gettimeofday(&start, NULL);

    int fileRd;
    if ((fileRd = open(fileName, O_RDONLY, 0644)) == -1) {
        perror("Error opening file");
        return 1;
    }
    int fileWr;
    if ((fileWr = open(destFileName, O_WRONLY | O_CREAT, 0644)) == -1) {
        perror("Error opening file");
        return 1;
    }

    size_t fileSize = fseek(fileRd, 0, SEEK_END);
    if (ftruncate(fileWr, fileSize) == -1) {
        perror("Error resizing destination file");
        close(fileRd);
        close(fileWr);
        return 1;    
    }

    void * addrRd;
    if((addrRd = mmap(NULL, fileSize, PROT_READ,MAP_PRIVATE, fileRd, 0)) == MAP_FAILED) {
        perror("Error mapping file");
        close(fileRd);
        close(fileWr);
        return 1;
    }    
    void * addrWr;
    if((addrWr = mmap(NULL, fileSize, PROT_WRITE, MAP_SHARED, fileWr, 0)) == MAP_FAILED) {
        perror("Error mapping file");
        munmap(addrRd, fileSize);
        close(fileRd);
        close(fileWr);
        return 1;
    } 

    memcpy(addrWr, addrRd, fileSize);
    gettimeofday(&end, NULL);


    if (munmap(addrRd, fileSize) == -1){
        perror("Error unmapping rd file");
        close(fileRd);
        close(fileWr);
        return 1;
    }
    if (munmap(addrWr, fileSize) == -1){
        perror("Error unmapping wr file");
        close(fileRd);
        close(fileWr);  
        return 1;
    }

    close(fileRd);
    close(fileWr);
    printf("\tCopying file with mmap took %.6f seconds\n", getTimeElapsed(start, end));
    return 0;
}

double getTimeElapsed(struct timeval start, struct timeval end) {return (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_usec - start.tv_usec)/1000000;}
