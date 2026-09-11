#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

void process_command(std::string command) {
    // comandos internos
    if (command == "exit")
        exit(0);

   
    // caminho onde serão procurados os binarios /bin/comando
    std::string absolute_path = "/bin/" + command;

    if (access(absolute_path.c_str(), F_OK) == 0) { // existe o arquivo?
        if (access(absolute_path.c_str(), X_OK) == 0) { // esse arquivo que existe tem permissão para executar?
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

                execve(absolute_path.c_str(), argv, NULL); // o filho vai rodar o /bin/comando no outro fork 
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

int main() {
    while (true) {
        std::cout << "vi$h> "; // acho que esse nome ta muito grande

        std::string command;
        getline(std::cin, command); // pega a linha inteira, ou seja até o ENTER

        process_command(command); // chama a função la de cima
    }

    return 0;
}