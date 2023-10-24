#define _GNU_SOURCE
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int scan_dir(char*, char*);

int main(int argc, char **argv)
{
    
    if(argc < 2)                //перевірка на кіль-ть аргументів
    {                           //3 аргумент - ім'я директорії - опціонально
        printf("Too few arguments\n");
        return 1;
    }

    char *path = ".";

    int fr;     //file read
    size_t bytes;           //кіль-ть прочитаних байт
    if((fr = open(argv[1], O_RDONLY)) == -1)    //перевірка відкриття файлу для читання
        {
            printf("Cannot open sign file %s\n", argv[1]);
            return 1;
        }
    if((bytes = lseek(fr, 0, SEEK_END)) == -1)      //перевірка 
        {
            printf("Error procesing %s\n", argv[1]);
            return 1;
        }
    lseek(fr, 0, SEEK_SET); //повертаюсь на початок файлу
    char* PIBN = malloc(bytes);
    read(fr, PIBN, bytes);
    close(fr);

    if(argc > 2)
        path = argv[2];
    scan_dir(path, PIBN);
        
    
    free(PIBN);
    return 0;
}

int scan_dir(char* path, char* PIBN)
{
    DIR *dir;
    int fr, file_size;
    struct dirent *entry;
    char *buffer;
    if((dir = opendir(path)) == NULL)
        {
            printf("Error opening directory %s", path);
            return 1;
        } 
    while((entry = readdir(dir)) != NULL)
        if(entry->d_type == DT_DIR) 
        {
            if(strcmp(entry->d_name, ".") || strcmp(entry->d_name, ".."))
            if((buffer = (char*)malloc((strlen(path) + strlen(entry->d_name) + 1)* sizeof(char)+1 )) == NULL)
                {
                    printf("Unable allocate a memory \n");
                    return 1;
                }
                strcpy(buffer, path);
                strcat(buffer, "/");
                strcat(buffer, entry->d_name);
                scan_dir(buffer, PIBN);
                free(buffer);
        }
        else if(entry->d_type == DT_REG)
        {
            //if((fr = open(entry->d_name, O_RDONLY)) == -1)    //перевірка відкриття файлу для читання
            char fullPath[PATH_MAX];
            snprintf(fullPath, PATH_MAX, "&s&s", path, entry->d_name);
            if((fr = open(fullPath, O_RDONLY)) == -1)    //перевірка відкриття файлу для читання
                printf("Cannot open input file %s\n", entry->d_name);
            if((file_size = lseek(fr, 0, SEEK_END)) == -1)      //перевірка 
                printf("Error procesing %s\n", entry->d_name);
            //while(ftell(fr) < file_size - strlen(PIBN))
                for(int i = 0; i < entry->d_reclen / sizeof(char) - strlen(PIBN); i++){
                if(fgetc(fr) == PIBN[0])
                { 
                    int i = 1;
                    while(fgetc(fr) == PIBN[i])
                        i++;
                    if (i == strlen(PIBN))
                        printf(" PIBN founded in file %s", entry->d_name);
                    else
                        lseek(fr, -i, SEEK_CUR);
                }
                }
            close(fullPath);
            
        }
    


}