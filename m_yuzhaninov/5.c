#include <stdio.h>
// Для взаимодействия с файлом
#include <unistd.h>
// Для открытия файла
#include <fcntl.h>
#include <stdlib.h>

int main()
{
    // Переменная для записи файлового дискриптора
    int fd;
    // Текст содержащий начало каждой строки
    long text[1000];  
    // Переменная для хранения позиции
    long pos = 0; 
    // Переменная для записи длинны строки
    int len = 0;
    char symbol;
    // Массив содержащий длины строк
    int lengths[1000];
    // Количество строк
    int count = 0;

    // Открывает файл для чтения
    fd = open("data.txt", O_RDONLY);

    // Первая строка начинается с нулевого отступа
    text[0] = 0;

    // Читаем по одному байту из файла, чтобы найти символ переноса строки
    while (read(fd, &symbol, 1) > 0)
    {
        pos++;
        len++;
        if (symbol == '\n')
        {
            // Записываем длинну текущей строки
            lengths[count] = len;
            count++;
            // Записываем начало следующей строки
            text[count] = pos;
            len = 0;
        }
    }

    // Если последняя строка не заканчивается символом переноса строки
    if (len > 0)
    {
        // Записываем длинну
        lengths[count] = len;
        count++;
    }

    while (1)
    {
        int n;
        printf("Введите номер строки: ");
        scanf("%d", &n);

        // Если 0, то завершаем программу
        if (n == 0)
            break;

        // Проверка на корректность
        if (n < 1 || n > count)
        {
            printf("Нет такой строки.\n");
            continue;
        }

        // Смещаем курсор до начала нужной строки
        // SEEK_SET говорит что отсчет идет от начала файла
        lseek(fd, text[n - 1], SEEK_SET);

        // Выделяем память под строку и читаем ее
        char *line = malloc(lengths[n - 1] + 1);
        read(fd, line, lengths[n - 1]);
        // Убираем символ переноса строки
        if (line[lengths[n - 1] - 1] == '\n')
        {
            line[lengths[n - 1] - 1] == '\0';
        }
        else 
        {
            line[lengths[n - 1]] == '\0';
        }

        printf("Строка %d: %s\n", n, line);
        free(line);
    }

    close(fd);
    return 0;
}
