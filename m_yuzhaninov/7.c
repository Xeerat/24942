#include <stdio.h>
// Для взаимодействия с файлом
#include <unistd.h>
// Для открытия файла
#include <fcntl.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/mman.h>   
#include <sys/stat.h> 
#include <string.h>

int main()
{
    // Переменная для записи файлового дискриптора
    int fd;
    // Текст содержащий начало каждой строки
    long text[1000];  
    // Массив содержащий длины строк
    int lengths[1000];
    // Количество строк
    int count = 0;

    // Открывает файл для чтения
    fd = open("data.txt", O_RDONLY);

    // Структура для хранения информации о файле
    struct stat st;
    // Добавляем информацию о файле
    fstat(fd, &st); 
    // Получаем размер файла в байтах
    long long filesize = st.st_size;

    // Отображаем файл в память
    // Первый аргумент это адрес в памяти, если NULL значит определит сам
    // Второй аргумент это размер который нужно отобразить
    // Третий тип взаимодействия с файлом, в данном случае только для чтения
    // Четвертый говорит о том, что изменения в памяти не меняют файл
    // Пятый это дескриптор файла
    // Шестой смещение с начала файла
    char *data = mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, fd, 0);

    // Нулевой отступ
    text[0] = 0;
    // Переменная для обозначения начала строки
    long long start = 0;

    // Заполнение массивов
    for (long long i = 0; i < filesize; i++)
     {
        if (data[i] == '\n') 
        {
            lengths[count] = i - start + 1;
            count++;
            text[count] = i + 1;
            start = i + 1;
        }
    }
    // Обработка последней строки 
    if (start < filesize) 
    {
        lengths[count] = filesize - start;
        count++;
    }

    while (1)
    {
        int n;
        printf("Введите номер строки: ");
        // Принудительно выводим буфер stdout
        fflush(stdout);

        // Создаем множество файловых дескрипторов
        fd_set fds;
        // Структура для паузы 
        struct timeval tv;
        // Очищаем множество
        FD_ZERO(&fds);
        // Добавляем дескриптор stdin, чтобы за ним можно было следить
        FD_SET(0, &fds);
        // Ставим количество секунд для паузы
        tv.tv_sec = 5;
        // Количество микросекунд
        tv.tv_usec = 0;

        // Ждем событие на дескрипторах
        // Первый аргумент это максимальный дескриптор + 1
        // Второй это множество дескрипторов для чтения 
        // Следующие два отвечают за запись и ошибки
        // Последний это пауза
        int retval = select(1, &fds, NULL, NULL, &tv);

        // Если вернулся 0, то пользователь ничего не ввел
        if (retval == 0)
        {
            printf("\nВремя вышло. Содержимое файла:\n\n");
            // Выводим весь файл
            write(1, data, filesize);
            break;
        }
        // Иначе 
        else
        {
            // Ввод остается в буфере stdin и scanf читает оттуда номер строки
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

            // Определяем длинну строки и выделяем память
            int len = lengths[n - 1];
            char *line = malloc(len + 1);

            // Копируем нужную строку из памяти
            memcpy(line, data + text[n - 1], len);
            
            // Корректировка
            if (len > 0 && line[len - 1] == '\n') 
            {
                line[len - 1] = '\0';
            }
            else
            {
                line[len] = '\0';
            }

            printf("Строка %d: %s\n", n, line);
            free(line);
        }
    }
    // Освобождаем память
    munmap(data, filesize);
    close(fd);
    return 0;
}
