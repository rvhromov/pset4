#include <cs50.h>
#include <stdio.h>
#include <stdlib.h>

#define SIZE 512

bool check_amount_of_arguments(int argc);
bool check_file_reading(FILE* file);
int get_jpeg(char* argv);
void close_file(FILE* f);

int main(int argc, char* argv[])
{
    // Количество входных параметров должно быть 2, иначе ошибка 1
    if (check_amount_of_arguments(argc))
        return 1;
    // Файл должен коректно открываться, иначе ошибка 2
    if (get_jpeg(argv[1]))
        return 2;
}

// Проверить количество аргументов
bool check_amount_of_arguments(int argc)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./recover image\n");
        return true;
    }
    
    return false;
}

// Проверить файл на чтение
bool check_file_reading(FILE* file)
{
    if (file == NULL)
    {
        fprintf(stderr, "Could not open file\n");
        return true;
    }
    
    return false;
}

// Найти JPEG файлы
int get_jpeg(char* argv)
{
    // Буфер для хранения блока памяти в 512 байт
    unsigned char buffer [SIZE] = {0};
    // Имя и формат файла
    char jpeg_name [8] = {0};
    // Числовое имя файла
    int name_counter = 0;
    
    // Входящий и исходящий файлы
    FILE* file = fopen(argv, "r");
    FILE* img = NULL;
    
    // Проверить файл на чтение
    if (check_file_reading(file))
        return 2;
    
    // Пока не конец входящего файла
    while (fread(buffer, SIZE, 1, file))
    {
        // Если найдено начало нового JPEG
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0)
        {
            // Закрыть предыдущий файл, если он открыт
            close_file(img);
            // Установить имя для нового файла    
            sprintf(jpeg_name, "%03i.jpg", name_counter++); 
            // Открыть для записи новый файл
            img = fopen(jpeg_name, "w");
        }
        
        // Пока файл открыт записывать в него порции по 512 байт
        if (img != NULL)
            fwrite(buffer, SIZE, 1, img);
    }
    
    // Закрыть все файлы 
    close_file(img);
    close_file(file);
        
    return 0;
}

// Закрыть файл
void close_file(FILE* f)
{
    if (f != NULL)
        fclose(f);
}