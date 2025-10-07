#include <stdio.h>
#include <unistd.h>

void print_uid()
{
    printf("Реальный идентификатор пользователя: %d\n", (int)getuid());
    printf("Эффективный идентификатор пользователя: %d\n\n", (int)geteuid());
}

void open_file(const char *path)
{
    FILE *data;
    data = fopen(path, "r");

    if (data != NULL)
    {
        printf("Файл удалось открыть\n\n");
        fclose(data);
    }
    else
    {
        perror("Произошла ошибка: ");
    }
}

int main()
{
    const char *path = "data.txt";

    print_uid();
    open_file(path);

    if (setuid(getuid()) != 0)
    {
        perror("Произошла ошибка:");
    }

    print_uid();
    open_file(path);

    return 0;
}