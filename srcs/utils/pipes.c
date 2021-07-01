#include "minishell.h"

char **pipe_next_cmd_recorder(t_job *job)
{
    char **cmd_array;
    int len;
    char **args;
    char *cmd;

    cmd = job->pipe_next->redir->command;
    args = job->pipe_next->redir->args;
    int i = 1;
    int j = 0;
    if (args == NULL)// если аргумент всего 1 (то это сама команда) то пишем его в массив env и 1 ячейку резерв для терминатора
    {
        cmd_array = (char **) malloc(sizeof(char *) * (1 + 1));
        cmd_array[0] = cmd;
    }
    else// если аргументов не один
    {
        len = how_many_lines((char **) args);// считаем кол-во аргументов
        cmd_array = (char **) malloc(sizeof(char *) * (len + 1+1)); // выделяем память + 1 для ноля и +1 для команды
        cmd_array[0] = cmd;
        while (args[j] != NULL) // запись из оригинала в замолоченный двумерный массив с размером рядов оригинала после записи команды
        {
            cmd_array[i] = args[j];// лучше записывать через индекс// i = 1 так как в ноль записана команда, j = 0
            j++;
            i++;
        }
    }
    cmd_array[i]=NULL;// в конце добав терминатор
    return(cmd_array);
}

char ***pipe_cmd_args_recorder(t_main *main) // запись команд и их аргументов в 2м массив
{
    int i;
    int c_num;


    char ***cmds;
    int p_num;

//    c_num = how_many_lines(main->job); // подсчет комманд
    c_num = 2;
    cmds = (char ***) malloc(sizeof(char *) * (c_num + 1));
    i = 0;
    p_num = c_num - 1;
    if (p_num == 1)
    {
        cmds[0] = cmd_args_to_argv_recorder(main);
        cmds[1] = pipe_next_cmd_recorder(main->job);
        cmds[2] = NULL;
        return(cmds);
    }
    if (p_num > 1)
    {
        while (main->job->job_next) {
            main->job = main->job->job_next;
            cmds[i] = cmd_args_to_argv_recorder(main);
            if (main->job->pipe_next)
                cmds[i++] = pipe_next_cmd_recorder(main->job);
            i++;
        }
        cmds[i] = NULL;
    }
    return(cmds);
}

void connect_stdio_to_pipes(int prev_fds[], int next_fds[])
{
    if (prev_fds[0] >= 0) // если есть что-то в нулевом фд (вход) или ноль ()
    {
        dup2(prev_fds[0], 0); // дублируем нулевой в ноль (запись в файл) // вливание в трубу
        close(prev_fds[0]);
        close(prev_fds[1]);
    }
    if (next_fds[1] >= 0)// след 1 фд на выход имеет что-то
    {
        dup2(next_fds[1], 1); // дублируем 1 в 1 (письмо из файла = выход) выливание из трубы
        close(next_fds[1]);
        close(next_fds[0]);
    }
}

void execute_pipes (t_main *main)
{
    int length;
    char ***commands;
    int prev_pipe_fds[2]; // объявляем массив предыдущ файловых дескрипторов
    int next_pipe_fds[2];

    commands = pipe_cmd_args_recorder(main);
    length = how_many_lines(*commands); // кол-во комманд
    int i = 0; // итератор
    next_pipe_fds[0] = -1; // инициализация  значения следующих файловых дескрипторов нулевого элемента
    next_pipe_fds[1] = -1;
    while (i < length) // пока не прошлись по всем командам
    {
        prev_pipe_fds[0] = next_pipe_fds[0]; // предыдущ нулевой присваивает значение след нулевого
        prev_pipe_fds[1] = next_pipe_fds[1];
        if (i != length - 1) // если кол-во команд не равно количеству команд-1 (те кол-ву пайпов)
            pipe(next_pipe_fds);// создаем каналы (трубы для следующих команд)
        else // иначе если команды равны пайпам
        {
            next_pipe_fds[0] = -1; // инициализируем на -1
            next_pipe_fds[1] = -1;
        }
        if (fork() == 0) // в дочери
        {
            connect_stdio_to_pipes(prev_pipe_fds, next_pipe_fds); // соединяем предыд в следующие
            char ***cmd = commands[i]; // команду пишем в двумерный для подачи в execve
            execve(cmd[0], cmd, NULL);// исполняем в дочери
            //exit(127);// не нужен exit, так как  дочерний процесс сам себя зачищает
        }
        close(prev_pipe_fds[0]); // закрываем вход предыдущ
        close(prev_pipe_fds[1]); // закрываем вход предыдущ
        i++;
    }
    wait(NULL); // один wait может ждать несколько процессов // без pid
    //wait(NULL);
    //wait(NULL);
    // wait(NULL);
//    return (0);

}