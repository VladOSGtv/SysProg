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
    // if (remove(fileName) == -1) 
    //     perror("Error deleting file");

    // Копіювання файлу
    char* destFileName = argv[3];
    //snprintf(destFileName, sizeof(destFileName), "%s_copy", fileName);

    cpyfLowLevFunIO(fileName, destFileName);
    if (remove(destFileName) == -1) 
        perror("Error deleting file");

    cpyDefFunIO(fileName, destFileName);
    if (remove(destFileName) == -1) 
        perror("Error deleting file");

    cpyByMmap(fileName, destFileName);
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
    if ((fd = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1) {
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

    unsigned char * addr;
    if((addr = (unsigned char*) mmap(0, fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
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

int cpyfLowLevFunIO(char* fileName, char* destFileName)
{
    struct timeval start, end;
    gettimeofday(&start, NULL);

    int fileRd;
    if ((fileRd = open(fileName, O_RDONLY)) == -1) {
        perror("Error opening file");
        return 1;
    }
    int fileWr;
    if ((fileWr = open(fileName, O_WRONLY | O_CREAT | O_TRUNC)) == -1) {
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

    printf("Copying file with low-level I/O took %.6f seconds\n", getTimeElapsed(start, end));
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

    printf("Copying file with standard file I/O took %.6f seconds\n", getTimeElapsed(start, end));
    return 0;
}
int cpyByMmap(char* fileName, char* destFileName)
{
    struct timeval start, end;
    gettimeofday(&start, NULL);

    int fileRd;
    if ((fileRd = open(fileName, O_RDONLY)) == -1) {
        perror("Error opening file");
        return 1;
    }
    int fileWr;
    if ((fileWr = open(fileName, O_WRONLY | O_CREAT | O_TRUNC)) == -1) {
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
    if((addrRd = mmap(0, fileSize, PROT_READ,MAP_SHARED, fileRd, 0)) == MAP_FAILED) {
        perror("Error mapping file");
        close(fileRd);
        close(fileWr);
        return 1;
    }    
    void * addrWr;
    if((addrWr = mmap(0, fileSize, PROT_WRITE, MAP_SHARED, fileWr, 0)) == MAP_FAILED) {
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
    printf("Copying file with mmap took %.6f seconds\n", getTimeElapsed(start, end));
    return 0;
}

double getTimeElapsed(struct timeval start, struct timeval end) {return end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) / 1000000;}