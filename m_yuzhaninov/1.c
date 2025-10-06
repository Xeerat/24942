#include <stdio.h>
#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>


extern char **environ;


void print_limit(const char *name, int resource)
{
    struct rlimit rl;
    getrlimit(resource, &rl);
    printf("%s: soft = %ld, hard = %ld\n", name, (long)rl.rlim_cur, (long)rl.rlim_max);
}


int main(int argc, char* argv[])
{
    int idx_opt=0;
    int opt=0;
    pid_t pid;
    struct rlimit rl; 
    long new;
    char path[1000];
    char **env = environ;
    char *arg = NULL;
    char *eq = NULL;

    struct option long_options[] = 
    {
        {"Unew_ulimit", required_argument, 0, 'U'},
        {"Csize", required_argument, 0, 'C'},
        {"Vname", required_argument, 0, 'V'},
        {0, 0, 0, 0}
    };

    while((opt = getopt_long(argc, argv, "is:puU:cC:dvV:", long_options, &idx_opt)) != -1)
    {
        switch (opt)
        {
            case 'i':
                printf("real UID: %d\n", (int)getuid());
                printf("effective UID: %d\n", (int)geteuid());
                printf("real GID: %d\n", (int)getgid());
                printf("effective GID: %d\n", (int)getegid());
                break;
            case 's':
                pid = atoi(optarg);
                setpgid(pid, pid);
                printf("Процесс %d стал лидером группы: %d\n", pid, getpgid(pid));
                break;
            case 'p':
                pid = getpid();
                pid_t ppid = getppid();
                pid_t pgid = getpgid(0);
                printf("Текущий процесс: %d\nРодитель текущего процесса: %d\nИдентификатор группы процессов: %d\n", pid, ppid, pgid);
                break;
            case 'u':
                print_limit("Ограничение по времени работы процесса", RLIMIT_CPU);
                print_limit("Ограничение на размер файлов", RLIMIT_FSIZE);
                print_limit("Ограничение на размер сегмента данных", RLIMIT_DATA);
                print_limit("Ограничение на размер стека", RLIMIT_STACK);
                print_limit("Ограничение на количество открытых файлов", RLIMIT_NOFILE);
                print_limit("Ограничение на размер адресного пространства", RLIMIT_AS);
                print_limit("Ограничение на размер core-файла", RLIMIT_CORE);
                break;
            case 'U':
                new = atol(optarg);
                getrlimit(RLIMIT_NOFILE, &rl);

                rl.rlim_cur = new;
                setrlimit(RLIMIT_NOFILE, &rl);
                break;

            case 'c':
                print_limit("Ограничение на размер core-файла", RLIMIT_CORE);
                break;

            case 'C':
                new = atol(optarg);
                getrlimit(RLIMIT_CORE, &rl);
                
                rl.rlim_cur = new;
                setrlimit(RLIMIT_CORE, &rl);
                break;
            
            case 'd':
                getcwd(path, sizeof(path));
                printf("Текущая директория: %s\n", path);
                break;

            case 'v':
                while (*env)
                {
                    printf("%s\n", *env);
                    env++;
                }
                break;
            
            case 'V':
                *arg = optarg;
                *eq =strchr(arg, '=');
                *eq = '\0';
                const char *name = arg;
                const char *value = eq + 1;
                setenv(name, value, 1);
                break;

            case '?':
                printf("Такой опции нет");
                break;
        }
    }

    return 0;
}