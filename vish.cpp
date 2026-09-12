#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdexcept>
#include <sys/stat.h>
#include <stdio.h>
#include <algorithm>

void process_command(std::string command) {

    if (command == "sair") {
        exit(0);
    }

    if (command == "vish --versao") {
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
            std::cout << "permission denied: " << command << std::endl;
        }

    } else {
        std::cout << "Command not found: " << command << std::endl;
    }
}

int main() {

    while (true) {

        std::cout << "\033[1;33mpoti$h🦐 \033[1;36m❯ \033[0m";

        std::string command;
        getline(std::cin, command);

        if (!command.empty()) {
            process_command(command);
        }
    }

    return 0;
}
