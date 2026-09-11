#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdexcept> 
#include <sys/stat.h> 
#include <stdio.h>

void process_command(std::string command) {
    // comandos internos
    if (command == "exit")
        exit(0);

    if (command == "vish --version")
        std::cout << "1.0" << '\n';
   
    // caminho onde serão procurados os binarios /bin/comando
    std::string absolute_path = "/bin/" + command;
    std::string temp_path = "/bin/temp/" + command;

    if (access(absolute_path.c_str(), F_OK) == 0 || access(temp_path.c_str(), F_OK) == 0) { // existe o arquivo?
        if (access(absolute_path.c_str(), X_OK) == 0 || access(temp_path.c_str(), X_OK) == 0) { // esse arquivo que existe tem permissão para executar?
            pid_t pid = fork(); // cria o clone

        if (pid < 0) { // o fork retorna 0 ou 1 se deu menor que 0 aconteceu erro
            // Erro
            std::cout << "Erro de execução!" << std::endl;
            return;
        } else if (pid == 0) { // se ele retornou 0 é o filho
            char* argv[2] = {
                (char*)command.c_str(), // como as chamadas de sistemas foram feitas em C temos que transformar a string em um array de char e é isso que o c_str faz transforma o std::string em uma string C
                nullptr
            };
                if (access(absolute_path.c_str(), F_OK) == 0)
                {
                    execve(absolute_path.c_str(), argv, NULL); // o filho vai rodar o /bin/comando no outro fork 
                } else {
                    execve(temp_path.c_str(), argv, NULL);
                }
            } else {
                waitpid(pid, nullptr, 0); // enquanto o isso o pai so espera o filho terminar
            }
        } else {
            std::cout << "permission denied: " << command << std::endl; // se nao tem permissão
        }
    } else {
        std::cout << "Command not found: " << command << std::endl; // se nao existe
    }
}



std::string ocultaDiretorio(const std::string& nomeDir) {
    if (!nomeDir.empty() && nomeDir[0] == '.') {
        return nomeDir;
    }
    return "." + nomeDir;
}




// TODO: PESQUISAR 
// como rodar em sudo
// criar o diretorio com o syscall mkdir?
// criar um fork para rodar os comandos linux?
// ou usar os syscall para fazer tudo?
void criaPastaOculta(char* nomeDir){
    if (nomeDir == NULL || strlen(nomeDir)) {
        throw std::invalid_argument("o valor nao pode ser nulo nem vazio");
    }
    if (nomeDir[0] == '.') { // ja ta oculto
        mkdir(nomeDir, 0777);
    } else {
        std::string diretorioPath = ocultaDiretorio(nomeDir);
        mkdir(diretorioPath.c_str(), 0777);
    }
}













int main() {
    while (true) {
        std::cout << "vi$h> "; // acho que esse nome ta muito grande

        std::string command;
        getline(std::cin, command); // pega a linha inteira, ou seja até o ENTER

        process_command(command); // chama a função la de cima
    }

    return 0;
}