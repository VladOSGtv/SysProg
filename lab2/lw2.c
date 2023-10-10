#include <stdio.h>
#include <stdlib.h>
int main(int argc, char **argv)
{
    FILE *fin;      //файлове введення
    if((fin = fopen(argv[1], "r")) == NULL) //помилка
    {
        fprintf(stderr, "Error opening file &s", argv[1]);
        return 1;
    }

    fseek(fin, 0, SEEK_END);    //дізнаюсь кіль-символів - переміщуюсь в кінець
    int fSize = ftell(fin) - 1;    //дізнаюсь, на якому я місці      - 1 => "\0"
    if(fSize%2)
        fSize = (fSize + 1) /2;
    else
        fSize /= 2;             //беремо до уваги пробіли

    int *numsFromFile = (int*) malloc(fSize * sizeof(int)); //масив цифр з файлу

                if(numsFromFile == NULL){
                    perror("Error opening file");
                    return 1;
                }

    fseek(fin, 0, SEEK_SET);    //переміщуюсь в початок файлу для зчитування цифр
    for (int i = 0; i < fSize; i++)
        fscanf(fin, "%d", &numsFromFile[i]);
    fclose(fin);

    FILE *fout = fopen(argv[2], "w");   //виведення в файл

    char *ints[] = {"zero ", "one ", "two ", "three ", "four ", "five ", "six ", "seven ", "eight ", "NINE "};
    for(int i = 0; i < fSize; i++)  //текстове представлення цифр, для запису в файл
        fputs(ints[numsFromFile[i]], fout);
    
    char PIBN[] = " Kramar Vladislav Igorovich, 335a ";
        fseek(fin, -(5 + sizeof(PIBN)), SEEK_END);
    fprintf(fout, "%s", PIBN);

    fclose(fout);
    free(numsFromFile); //прибираю після себе
    return 0;
}                           