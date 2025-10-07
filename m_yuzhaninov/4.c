#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Узел
typedef struct Strings 
{
    // Память под строку выделяется динамически
    char *data;
    // Указатель на следующий узел списка
    struct Strings *next;
}S;

// Функция добавления нового узла в конец списка
void new_string(S **head, const char *str) 
{
    // Создаем новый узел
    S *new = (S *)malloc(sizeof(S));

    int len = strlen(str);
    // Выделяем память в узле под строку + '\0'
    new->data = (char *)malloc(len + 1); 

    // Копируем строку
    strcpy(new->data, str);
    // Указатель на следующий узел делаем NULL
    new->next = NULL;

    // Если ни одного узла в списке не было, то новый узел это первый
    if (*head == NULL) 
    {
        *head = new;
    } 
    else 
    {
        // Доходим до последнего узла
        S *p = *head;
        while (p->next != NULL)
        {
            p = p->next;
        }
        // Вставляем новый узел
        p->next = new;
    }
}

// Функция освобождения памяти списка
void free_list(S *head) 
{
    // Пока узлы существуют
    while (head) 
    {
        // Сохраняем текущий узел
        S *tmp = head;
        // Смещаемся на новый 
        head = head->next;
        // Удаляем сохраненный узел
        free(tmp->data);
        free(tmp);
    }
}

int main() 
{
    // Начало списка
    S *head = NULL;
    // Новая строка 
    char str[1000];

    printf("Введите строку\n");
    while (1) 
    {
        // Получаем строку
        fgets(str, 1000, stdin);

        // Удаляем символ новой строки
        int len = strlen(str);
        if (len > 0 && str[len - 1] == '\n')
        {
            str[len-1] = '\0';
        }

        // Конец цикла
        if (str[0] == '.') 
        {
            break; 
        }
        // Добавляем строку в список
        new_string(&head, str);
    }

    // Выводим строки из списка
    S *p = head;
    while (p) 
    {
        printf("%s\n", p->data);
        p = p->next;
    }

    // Очистка памяти
    free_list(head);

    return 0;
}
