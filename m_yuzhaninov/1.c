#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Взаимодействие с идентификаторами
#include <unistd.h>
// Получение лимитов
#include <sys/resource.h>
// Взаимодействие с длинными опциями и лимитами
#include <getopt.h>


// Внутренний массив содержащий все переменные среды
extern char **environ;

// Функция для вывода текущих и максимальных ограничение машины
void print_limit(const char *name, int resource)
{
    struct rlimit rl;
    // Получаем конкретный ресурс и выводим его
    getrlimit(resource, &rl);
    printf("%s: soft = %ld, hard = %ld\n", name, (long)rl.rlim_cur, (long)rl.rlim_max);
}


int main(int argc, char* argv[])
{
    // Выносим все переменные, нельзя инициализировать внутри switch
    // Индекс длинной опции
    int idx_opt=0;
    // Переменная для хранения опций
    int opt=0;
    // Идентификатор пользователя
    pid_t pid;
    // Структура для хранения лимитов машины
    struct rlimit rl; 
    // Переменная для записи нового значения лимита
    long new;
    // Массив для хранения пути до текущей директории
    char path[1000];
    // Дополнительная переменная для хранения переменных среды
    char **env = NULL;
    // Переменная хранящая новую переменную среды
    char *arg = NULL;
    // Разделитель
    char *eq = NULL;
    // Название переменной среды 
    char *name = NULL;
    // Значение переменной среды
    char *value = NULL;

    // Специальная структура для длинных опций
    struct option long_options[] = 
    {
        // 1 - название длинной опции
        // 2 - есть ли аргумент у опции
        // 3 - возможность записать аргумент в переменную
        // 4 - короткое название опции
        {"Unew_ulimit", required_argument, 0, 'U'},
        {"Csize", required_argument, 0, 'C'},
        {"Vname", required_argument, 0, 'V'},
        // Обязательно обозначаем конец нулями
        {0, 0, 0, 0}
    };

    // Получаем опции и определяем конкретные 
    // Когда опции кончаться, getopt_long выдаст -1
    while((opt = getopt_long(argc, argv, "is:puU:cC:dvV:", long_options, &idx_opt)) != -1)
    {
        switch (opt)
        {
            // Печатаем реальные и эффективные идентификаторы пользователя и группы 
            case 'i':
                // Разница между ними в правах
                // Реальный это тот, который принадлежит пользователю
                // Эффективный это тот с которым пользователь что то делает (например идентификатор 0 это администратор)
                printf("Реальный UID: %d\n", (int)getuid());
                printf("Эффективный UID: %d\n", (int)geteuid());
                printf("Реальный GID: %d\n", (int)getgid());
                printf("Эффективный GID: %d\n", (int)getegid());
                break;
            // Получение процесса и его становление лидером группы
            case 's':
                // Получаем процесс. optarg - это аргумент переданный опции
                pid = atoi(optarg);
                // Ставим процесс, как лидера в группе из одного процесса
                setpgid(pid, pid);
                printf("Процесс %d стал лидером группы: %d\n", pid, getpgid(pid));
                break;
            // Получение идентификатора текущего процесса, родителя и идентификатора группы 
            case 'p':
                pid = getpid();
                pid_t ppid = getppid();
                pid_t pgid = getpgid(0);
                printf("Текущий процесс: %d\nРодитель текущего процесса: %d\nИдентификатор группы процессов: %d\n", pid, ppid, pgid);
                break;
            // Вывод всех ограничений машины
            case 'u':
                print_limit("Ограничение по времени работы процесса", RLIMIT_CPU);
                print_limit("Ограничение на размер файлов", RLIMIT_FSIZE);
                print_limit("Ограничение на размер сегмента данных", RLIMIT_DATA);
                print_limit("Ограничение на размер стека", RLIMIT_STACK);
                print_limit("Ограничение на количество открытых файлов", RLIMIT_NOFILE);
                print_limit("Ограничение на размер адресного пространства", RLIMIT_AS);
                print_limit("Ограничение на размер core-файла", RLIMIT_CORE);
                break;
            // Изменение ограничения на количество открытых файлов
            case 'U':
                new = atol(optarg);
                getrlimit(RLIMIT_NOFILE, &rl);

                rl.rlim_cur = new;
                setrlimit(RLIMIT_NOFILE, &rl);
                break;
            // Вывод ограничение на размер core-файла
            case 'c':
                print_limit("Ограничение на размер core-файла", RLIMIT_CORE);
                break;
            // Изменение ограничения на размер core-файла
            case 'C':
                new = atol(optarg);
                getrlimit(RLIMIT_CORE, &rl);
                
                rl.rlim_cur = new;
                setrlimit(RLIMIT_CORE, &rl);
                break;
            // Вывод абсолютного пути до текущей директории
            case 'd':
                if (getcwd(path, sizeof(path)) != NULL)
                {
                    printf("Текущая директория: %s\n", path);
                }
                break;
            // Вывод всех переменных среды
            case 'v':
                env = environ;
                // Пока есть переменные
                while (*env)
                {
                    printf("%s\n", *env);
                    env++;
                }
                break;
            // Создание новой переменной среды
            case 'V':
                // Получаем аргумент опции
                arg = optarg;
                // Находим разделитель имени и значения
                eq = strchr(arg, '=');
                if (!eq)
                {
                    printf("Неверный формат, нужно name=value\n");
                    break;
                }
                // Разделяем на имя и значение
                *eq = '\0';
                name = arg;
                value = eq + 1;
                // Создаем новую переменную среды
                setenv(name, value, 1);
                break;
            // Иначе - неизвестная опция
            default:
                printf("Такой опции нет\n");
                break;
        }
    }

    return 0;
}