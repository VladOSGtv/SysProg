#define _GNU_SOURCE
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define BUF_SIZE 4096 // розмір буферу

int scan_dir(char *, char *);
int scanFile(char *path, char *sign);   //сканування файлу за адресою path на наявність
                                        //сигнатури sign
int main(int argc, char **argv)
{

    if (argc < 2) // перевірка на кіль-ть аргументів
    {             // 3 аргумент - ім'я директорії - опціонально
        printf("Too few arguments\n");
        return 1;
    }

    char *path = ".";                       //шлях до директорії

    int fr;                                   // file read
    size_t bytes;                             // кіль-ть прочитаних байт
    if ((fr = open(argv[1], O_RDONLY)) == -1) // перевірка відкриття файлу для читання
    {
        printf("Cannot open sign file %s\n", argv[1]);
        return 1;
    }
    if ((bytes = lseek(fr, 0, SEEK_END)) == -1) // перевірка
    {
        printf("Error procesing %s\n", argv[1]);
        return 1;
    }
    lseek(fr, 0, SEEK_SET); // повертаюсь на початок файлу
    char *PIBN = malloc(bytes + 1);
    read(fr, PIBN, bytes);      //зчитую сигнатуру
    close(fr);
    PIBN[bytes] = '\0';         //нуль-термінатор
    printf(" signature: %s", PIBN); 
    if (argc > 2)
        path = argv[2];
    scan_dir(path, PIBN);

    free(PIBN);
    return 0;
}

int scan_dir(char *path, char *PIBN)
{
    DIR *dir;
    struct dirent *entry;
    char *buffer = NULL;
    if ((dir = opendir(path)) == NULL)
    {
        printf("Error opening directory %s", path);
        return 1;
    }
    else printf(" dir %s is opened \n", path);  //показую в яку саме директорію увійшов

    while ((entry = readdir(dir)) != NULL)
        if (entry->d_type == DT_DIR)    //якщо папка
        {
            if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))  
            {
                printf(" Dir %s is founded here \n", entry->d_name);

                if ((buffer = (char *)malloc((strlen(path) + strlen(entry->d_name) + 1) 
                *sizeof(char) + 1)) == NULL)
                {
                    printf("Unable allocate a memory \n");
                    return 1;
                }
                strcpy(buffer, path);
                strcat(buffer, "/");
                strcat(buffer, entry->d_name);
                scan_dir(buffer, PIBN);     //відкриваю директорію за новим шляхом
                free(buffer);
            }
        }
        else if (entry->d_type == DT_REG)   //якщо звичайний файл
        {
            printf(" File %s is founded here \n", entry->d_name);
            if ((buffer = (char *)malloc((strlen(path) + strlen(entry->d_name) + 1) * sizeof(char) + 1)) == NULL)
            {
                printf("Unable allocate a memory \n");
                return 1;
            }
            strcpy(buffer, path);
            strcat(buffer, "/");
            strcat(buffer, entry->d_name);
            scanFile(buffer, PIBN); //сканую файл за цим же шляхом

            free(buffer);
        }
    closedir(dir);
    return 0;
}

int scanFile(char *path, char *sign)
{
    char readBuffer[BUF_SIZE];  //буфер для зчитування
    int fr, file_size;

    if ((fr = open(path, O_RDONLY)) == -1)
    { // перевірка відкриття файлу для читання
        printf("Cannot open input file %s \n", path);
        return 1;
    }

    if ((file_size = lseek(fr, 0, SEEK_END)) == -1)
    { // перевірка
        printf("Error procesing %s\n", path);
        return 1;
    }
    int n = sizeof(sign) * 4;
    int num = 0;        //відправна точка, який саме символ сигнатури треба шукати
    int bytes;
    lseek(fr, 0, SEEK_SET); //починаю зчитування файлу зпочатку
    for (bytes = read(fr, readBuffer, BUF_SIZE); bytes > n;) // читання файлу
    {
        for (int i = 0; i < bytes; i++) // перебір буферу
        {
            if (readBuffer[i] == sign[num]) //якщо елемент буферу збігся з 
            {                               //відправним елементом сигнатури
                if (bytes >= i + n) //перевірка, скільки залишилось до кінця буферу, 
                {                              //чи поміститься сигнатура
                    int j = 1;
                    for (; j < n - num; j++)//чи всі елементи співпадають, починаючи з відправного 
                        if (readBuffer[j + i] != sign[j + num]) // елемента сигнатури
                        {
                            num = 0;    //обнуляю відправний елемент
                            break;
                        }
                    if (j == n - num)
                    {
                        printf(" ****** PIBN found in %s \n", path);
                        close(fr);
                        return 0;
                    }
                }
                else    //перевірка на наявність першої частини сигнатури в кінці буферу
                {       
                    int j = 0;
                    for (; j < bytes - (i + n - 1); j++)
                        if (readBuffer[j + i] != sign[j])
                            break;  //нема
                    if (j == bytes - (i + n - 1))
                        num = j;  //в наступному буфері починаю пошук з цього елементу сигнатури
                }
            }
        }
    }

    close(fr);
    return 1;
}