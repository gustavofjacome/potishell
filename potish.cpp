#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdexcept>
#include <sys/stat.h>
#include <stdio.h>
#include <algorithm>
#include <string>

void comandosInternos(std::string comandoInterno) {

}





void process_command(std::string command) {

    if (command == "sair") {
        exit(0);
    }

    if (command == "ajuda") {
    std::cout << "poti$h🦐 - Comandos internos disponíveis:\n";
    std::cout << "  clear      Limpa a tela\n";
    std::cout << "  exit       Sai do shell\n";
    return;
}

    if (command == "potish --versao") {
        std::cout << "1.0" << '\n';
        return;
    }

    if (command == "limpar") {
        std::cout << "\033[2J\033[1;1H";
        return;
    }

    std::string absolute_path = "/bin/" + command;

    if (std::count(command.begin(), command.end(), '/') > 0) {
        absolute_path = command;
    }

    if (access(absolute_path.c_str(), F_OK) == 0) {
        if (access(absolute_path.c_str(), X_OK) == 0) {
            pid_t pid = fork();

            if (pid < 0) {
                std::cout << "Erro de execução!" << std::endl;
                return;

            } else if (pid == 0) {
                char* argv[2] = {
                    (char*)command.c_str(),
                    nullptr
                };
                execve(absolute_path.c_str(), argv, NULL);

            } else {
                waitpid(pid, nullptr, 0);
            }

        } else {
            throw std::runtime_error("erro: Comando não encontrado: " + command);
        }

    } else {
        throw std::runtime_error("erro: Sem permissão para executar: " + command);
    }
}

int main() {
    const std::string potishFormatado = "\033[1;33mpoti$h🦐 \033[1;36m❯ \033[0m";



    while (true) {

        std::cout << potishFormatado;

        std::string command;
        getline(std::cin, command);

        if (!command.empty()) {
            process_command(command);
        }
    }

    return 0;
}
