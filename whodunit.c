#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // Количество передаваемых аргументов должно быть 3
    if (argc != 3)
    {
        fprintf(stderr, "Usage: ./whodunit infile outfile\n");
        return 1;
    }

    // Входящий и исходящий файлы
    char *infile = argv[1];
    char *outfile = argv[2];

    // Открыть входящий файл для чтения
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    // Открыть исходящий файл для записи
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // Прочитать BITMAPFILEHEADER входящего файла
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // Прочитать BITMAPINFOHEADER входящего файла
    BITMAPINFOHEADER bi;
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
    
    // Записать BITMAPFILEHEADER в исходящий файл
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // Записать BITMAPINFOHEADER в исходящий файл
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // Определить отступ
    int padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // Пройти по строкам файла
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // Пройти по пикселям в строках
        for (int j = 0; j < bi.biWidth; j++)
        {
            RGBTRIPLE triple;

            // Прочитать RGB triple из входящего файла
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
            
            // Красный в ноль
            if(triple.rgbtRed == 0xff)
                triple.rgbtRed = 0x00;
            // Зеленый в ноль
            if(triple.rgbtGreen == 0xff)
                triple.rgbtGreen = 0x00;
        
            // Записать RGB triple в исходящий файл
            fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
        }

        // Позиционирование
        fseek(inptr, padding, SEEK_CUR);

        // Записать отступ
        for (int k = 0; k < padding; k++)
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
