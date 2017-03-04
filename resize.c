#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

#define RGBTRIPLESIZE 3

int main(int argc, char *argv[])
{
    // Количество передаваемых аргументов должно быть 4
    if (argc != 4)
    {
        fprintf(stderr, "Usage: ./resize n infile outfile\n");
        return 1;
    }
    
    // Во сколько раз увеличивать
    int n = atoi(argv[1]);
    
    // n должно быть целым числом (0; 100]
    if(n <= 0 || n > 100)
    {
        fprintf(stderr, "n must be in (0, 100]");
        return 1;
    }
 
    // Входящий и исходящий файлы
    char *infile = argv[2];
    char *outfile = argv[3];

    // Открыть входящий файл для чтения
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s\n", infile);
        return 2;
    }

    // Открыть исходящий файл для записи
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s\n", outfile);
        return 3;
    }

    // Прочитать BITMAPFILEHEADER входящего файла
    BITMAPFILEHEADER bf, new_bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // Прочитать BITMAPINFOHEADER входящего файла
    BITMAPINFOHEADER bi, new_bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // Входящий файл должен быть 24-битным несжатым BMP 4.0 файлом
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
    
    // Новые экземпляры структур BITMAPFILEHEADER и BITMAPINFOHEADER
    new_bf = bf;
    new_bi = bi;
    
    // Новая ширина и высота 
    new_bi.biWidth = bi.biWidth * n;
    new_bi.biHeight = bi.biHeight * n;
    
    // Определить отступ
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int new_padding = (4 - (new_bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    // Новые размеры изображения и файла
    new_bi.biSizeImage = ((sizeof(RGBTRIPLE) * new_bi.biWidth) + new_padding) * abs(new_bi.biHeight);
    new_bf.bfSize = new_bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    
    // Записать новый BITMAPFILEHEADER в исходящий файл
    fwrite(&new_bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // Записать новый BITMAPINFOHEADER в исходящий файл
    fwrite(&new_bi, sizeof(BITMAPINFOHEADER), 1, outptr);
    
    // Пройти по строкам файла
    for (int i = 0, biHeight = abs(new_bi.biHeight); i < biHeight; i++)
    {
        // Позиционирование курсора в начальное положение n раз
        // Записать по вертикали n раз
        if (i % n != 0)
            fseek(inptr, -(bi.biWidth * RGBTRIPLESIZE + padding), SEEK_CUR);
    
        // Пройти по пикселям в строках
        for (int j = 0; j < bi.biWidth; j++)
        {
            RGBTRIPLE triple;

            // Прочитать RGB triple из входящего файла
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
            
            // Записать RGB triple в исходящий файл n раз
            for (int l = 0; l < n; l++)
                fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
        }
        
        // Позиционирование
        fseek(inptr, padding, SEEK_CUR);

        // Записать отступ
        for (int k = 0; k < new_padding; k++)
        {
            fputc(0x00, outptr);
        }
    }

    // Закрыть входящий файл
    fclose(inptr);

    // Закрыть исходящий файл
    fclose(outptr);

    return 0;
}