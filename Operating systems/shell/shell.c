#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<fcntl.h>  
#include<errno.h>  

char*** tokenizer(char** command, int argc, int *pipe_amount, int **pipe_pos, int *trunc_amount, int **trunc_pos, int *read_amount, int **read_pos, int *append_amount, int **append_pos){
    int op_amount = 0;                                                            //Quantidade total de delimitadores
    char ***cmd = (char***) malloc(1 * sizeof(char**));                           //Vetor de comandos individualizados
    cmd[0] = &command[0];                                                         //Primeiro comando tem o mesmo início que a sequência completa

    for(int i = 0; i < argc; i += 1){                                             //Percorre a sequência completa de comandos
        
        if(!strcmp(command[i], "|")){                                             //Busca por pipes '|'
            *pipe_amount += 1;                                                    //Itera-se a quantidade de '|'
            op_amount += 1;                                                       //Itera-se a quantidade de operadores
            cmd = realloc(cmd, (op_amount + 1) * sizeof(*cmd));                   //Aloca-se espaço para mais um comando
            *pipe_pos = (int*) realloc(*pipe_pos,
                                      (*pipe_amount) * sizeof(int));              //Aloca-se espaço para mais uma poisção de '|'
            cmd[op_amount] = &command[i + 1];                                     //Adiciona-se o comando corrente ao vetor de comandos individuais
            (*pipe_pos)[(*pipe_amount) - 1] = op_amount;                          //Adiciona-se a posição corrente no vetor de posição de '|'
            command[i] = NULL;                                                    //A posição atual do operador recebe NULL
        
        }else if(!strcmp(command[i], ">")){                                       //Busca por output redirections '>' 
            *trunc_amount += 1;                                                   //Itera-se a quantidade de '>'
            op_amount += 1;                                                       //Itera-se a quantidade de operadores
            cmd = realloc(cmd, (op_amount + 1) *sizeof(*cmd));                    //Aloca-se espaço para mais um comando
            *trunc_pos = (int *) realloc(*trunc_pos,
                                        (*trunc_amount) * sizeof(int));           //Aloca-se espaço para mais uma poisção de '>'
            cmd[op_amount] = &command[i + 1];                                     //Adiciona-se o comando corrente ao vetor de comandos individuais
            (*trunc_pos)[(*trunc_amount) - 1] = op_amount;                        //Adiciona-se a posição corrente no vetor de posição de '>'
            command[i] = NULL;                                                    //A posição atual do operador recebe NULL
        
        }else if(!strcmp(command[i], "<")){                                       //Busca por input redirections '<'  
            *read_amount += 1;                                                    //Itera-se a quantidade de '<'
            op_amount += 1;                                                       //Itera-se a quantidade de operadores
            cmd = realloc(cmd, (op_amount + 1) *sizeof(*cmd));                    //Aloca-se espaço para mais um comando
            *read_pos = (int *) realloc(*read_pos,
                                       (*read_amount) * sizeof(int));             //Aloca-se espaço para mais uma poisção de '<'
            cmd[op_amount] = &command[i + 1];                                     //Adiciona-se o comando corrente ao vetor de comandos individuais
            (*read_pos)[(*read_amount) - 1] = op_amount;                          //Adiciona-se a posição corrente no vetor de posição de '<'
            command[i] = NULL;                                                    //A posição atual do operador recebe NULL
        
        }else if(!strcmp(command[i], ">>")){                                      //Busca por output redirections '>>'  
            *append_amount += 1;                                                  //Itera-se a quantidade de '>>'
            op_amount += 1;                                                       //Itera-se a quantidade de operadores
            cmd = realloc(cmd, (op_amount + 1) *sizeof(*cmd));                    //Aloca-se espaço para mais um comando
            *append_pos = (int *) realloc(*append_pos,
                                        (*append_amount) * sizeof(int));          //Aloca-se espaço para mais uma poisção de '>>'
            cmd[op_amount] = &command[i + 1];                                     //Adiciona-se o comando corrente ao vetor de comandos individuais
            (*append_pos)[(*append_amount) - 1] = op_amount;                      //Adiciona-se a posição corrente no vetor de posição de '>>'
            command[i] = NULL;                                                    //A posição atual do operador recebe NULL
        }
    }
    
    command[argc] = NULL;                                                         //Deve-se "fechar" a sequência de comandos com NULL
    return cmd;                                                                   //Retorna-se a sequência de comandos individuais
}

int main(int argc, char** argv){
    if(argc == 1){
        return 0;
    }
    
    char **command, ***cmd = NULL;                                                //Cadeia completa de comandos e vetor de comandos individualizados, respectivamente
    int *pipe_pos = NULL, *trunc_pos = NULL, *read_pos = NULL, *append_pos = NULL; //Vetores que armazenam as posições de cada operador
    int pipe_amount = 0, trunc_amount = 0, read_amount = 0, append_amount = 0;    //Inteiros que armazenam a quantidade de cada operador

    command = &argv[1];                                                           //Para evitar a string do executável, a cadeia de comandos inicia-se no argv[1]
    cmd = tokenizer(command, argc - 1,
                    &pipe_amount, &pipe_pos,
                    &trunc_amount, &trunc_pos,
                    &read_amount, &read_pos,
                    &append_amount, &append_pos);                                 //Separa-se a cadeia de comandos em comandos individuais

    int h = 0, j = 0, k = 0, l = 0;                                               //Iteram sobre os vetores de read e trunc, respectivamente
    int n = pipe_amount + 1;                                                      //Quantidade de comandos na cadeia de pipes (ignora-se os nomes de arquivo)
    int in = STDIN_FILENO;                                                        //Inicializa-se 'in' com a entrada padrão
    int status;                                                                   //Waitpid status        
    int file;                                                                     //Descritor de arquivo
    
    int i = 0;                                                                    //Comando corrente
    for(int cont = 0 ; cont < n; cont += 1){                                      //Executa até o penúltimo comando
        int fd[2];                                                                //Descritor para o pipe
        pipe(fd);                                                                 //Cria um novo pipe
        
        pid_t pid;
        pid = fork();                                                             //Clona-se o processo (cria-se um filho)
        
        if(pid > 0){                                                              //Processo pai 
            close(fd[1]);                                                         //A saída do pipe não é necessária
            close(in);                                                            //A entrada anterior não é necessária
            in = fd[0];                                                           //A entrada do próximo processo filho será a do pipe atual

            //sort < in | grep teste
            if(pipe_amount > 0 && (pipe_pos[h] == i + 1 || pipe_pos[h] == i + 2)){//Itera-se sobre o vetor de append e sobre o comando corrente
                h += 1;
            }
            
            if(read_amount > 0 && read_pos[j] == i + 1){                          //Itera-se sobre o vetor de read e sobre o comando corrente
                j += 1;
                i += 1;
            }
            
            if(trunc_amount > 0 && trunc_pos[k] == i + 1){                        //Itera-se sobre o vetor de trunc e sobre o comando corrente
                k += 1;
                i += 1;
            }
            
            if(append_amount > 0 && append_pos[l] == i + 1){                      //Itera-se sobre o vetor de append e sobre o comando corrente
                l += 1;
                i += 1;
            }
            
            waitpid (pid, &status, 0);                                            //Aguarda-se execução processo filho
        }

        else if(pid == 0){                                                        //Processo filho
            close(fd[0]);                                                         //A entrada do pipe atual não é necessária
           
            if(read_amount > 0 && read_pos[j] == i + 1){                          //O processo filho recebe um arquivo de entrada
                file = open(cmd[i+1][0], O_RDONLY);                               //Abre-se o arquivo apenas para leitura
                if(file == -1){
                    printf("Arquivo ou diretório não encontrado\n");              //Trata-se exceção de abertura de arquivo
                    return -1;
                }
                fclose(stdin);                                                    //Fecha-se a entrada padrão
                dup2(file, STDIN_FILENO);                                         //Redireciona-se a entrada para o arquivo
                close(file);                                                      //Fecha-se o arquivo
            }else{                                                                //O processo filho lê da entrada setada anteriormente
                dup2(in, STDIN_FILENO);
            }
            
            if(pipe_amount > 0 && pipe_pos[h] == i + 1){
                if(cont == n - 1){
                    dup2(STDOUT_FILENO, STDOUT_FILENO);                           //O processo filho escreve na saída padrão
                }else{
                    dup2(fd[1], STDOUT_FILENO);                                   //O processo escreve na saída do pipe atual
                }
            }
            else if(trunc_amount > 0 && trunc_pos[k] == i + 1){                   //O processo filho recebe um arquivo de saída
                file = open(cmd[i+1][0], O_RDWR | O_CREAT | O_TRUNC, 0777);       //Abre-se o arquivo para leitura, escrita, truncamento e criação, com as devidas permissões
                if(file == -1){
                    printf("Arquivo ou diretório não encontrado\n");              //Trata-se exceção de abertura de arquivo
                    return -1;
                }
                fclose(stdout);                                                   //Fecha-se a saída padrão
                dup2(file, STDOUT_FILENO);                                        //Redireciona-se a saída para o arquivo
                close(file);                                                      //Fecha-se o arquivo
            }
            else if(append_amount > 0 && append_pos[l] == i + 1){                 //O processo filho recebe um arquivo de saída
                file = open(cmd[i+1][0],  O_RDWR | O_APPEND | O_CREAT, 0777);     //Abre-se o arquivo para leitura, escrita e inserção ao final, com as devidas permissões
                if(file == -1){
                    printf("Arquivo ou diretório não encontrado\n");              //Trata-se exceção de abertura de arquivo
                    return -1;
                }
                fclose(stdout);                                                   //Fecha-se a saída padrão
                dup2(file, STDOUT_FILENO);                                        //Redireciona-se a saída para o arquivo
                close(file);                                                      //Fecha-se o arquivo
            
            }else if(trunc_amount > 0 && trunc_pos[k] == i + 2){                  //O processo filho recebe um arquivo de saída após um arquivo de entrada
                file = open(cmd[i+2][0], O_RDWR | O_CREAT | O_TRUNC, 0777);       //Abre-se o arquivo para leitura, escrita, truncamento e criação, com as devidas permissões
                if(file == -1){
                    printf("Arquivo ou diretório não encontrado\n");              //Trata-se exceção de abertura de arquivo
                    return -1;
                }
                fclose(stdout);                                                   //Fecha-se a saída padrão
                dup2(file, STDOUT_FILENO);                                        //Redireciona-se a saída para o arquivo
                close(file);                                                      //Fecha-se o arquivo
            
            }else if(append_amount > 0 && append_pos[l] == i + 2){                //O processo filho recebe um arquivo de saída após um arquivo de entrada
                file = open(cmd[i+2][0],  O_RDWR | O_APPEND | O_CREAT, 0777);     //Abre-se o arquivo para leitura, escrita e inserção ao final, com as devidas permissões
                if(file == -1){
                    printf("Arquivo ou diretório não encontrado\n");              //Trata-se exceção de abertura de arquivo
                    return -1;
                }
                fclose(stdout);                                                   //Fecha-se a saída padrão
                dup2(file, STDOUT_FILENO);                                        //Redireciona-se a saída para o arquivo
                close(file);                                                      //Fecha-se o arquivo
            
            //sort < in | grep teste
            }else{
                if(cont == n - 1){
                    dup2(STDOUT_FILENO, STDOUT_FILENO);                           //O processo filho escreve na saída padrão
                }else{
                    dup2(fd[1], STDOUT_FILENO);                                   //O processo escreve na saída do pipe atual
                }
            }

            if(execvp(cmd[i][0], cmd[i]) == -1){                                  //Executa-se o i-ésimo comando
                printf("comando não encontrado\n");                               //Trata-se exceção do execvp
                return -1;
            }
            exit(0);                                                              //Retorna-se execução para o processo pai

        }else{                                                                    //Trata-se exceção do fork
            perror("fork()");
            return -1;
        }
        i += 1;                                                                   //Itera-se sobre o comando corrente
    }
	
	free(cmd);
	free(trunc_pos);
	free(read_pos);
	free(append_pos);
	return 0;                                                                     //Fim da execução
}
