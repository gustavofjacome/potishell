#include "utils.h"

#include <algorithm>
#include <cstdlib>
#include <stdexcept>
#include <unistd.h>
#include <sys/wait.h>

extern char **environ; //variaveis de ambiente permitiu que eu conseguisse compilar com o g++ pelo o shell ela é passada como parametro no meu execve

std::vector<std::string> geraVetorAgumentos(std::string comando) {
    std::vector<std::string> argumentosString;
    size_t inicio = 0;
    size_t fim = comando.find(' ');

    while (fim != std::string::npos) {
        if (inicio != fim) {
            argumentosString.push_back(comando.substr(inicio, fim - inicio));
        }
        inicio = fim + 1; 
        fim = comando.find(' ', inicio); 
    }
    if (inicio < comando.length()) {
        argumentosString.push_back(comando.substr(inicio));
    }
    return argumentosString;
}

std::vector<char*> converterParaArgv(std::vector<std::string>& args) {
    std::vector<char*> argv;
    for (size_t i = 0; i < args.size(); ++i) {
        argv.push_back(const_cast<char*>(args[i].c_str()));
    }
    argv.push_back(nullptr);
    return argv;
}

void verificacoesErros(const std::string& absolute_path, const std::string& programa) {
    if (access(absolute_path.c_str(), F_OK) != 0) {
        throw std::runtime_error("poti$h erro: Comando não encontrado: " + programa);
    }
    if (access(absolute_path.c_str(), X_OK) != 0) {
        throw std::runtime_error("poti$h erro: Sem permissão para executar: " + programa);
    }
}

void executarComandosExterno(std::vector<std::string>& args) {
    std::string programa = args[0];
    std::string absolute_path = "/bin/" + programa;

    if (std::count(programa.begin(), programa.end(), '/') > 0) {
        absolute_path = programa;
    }

    verificacoesErros(absolute_path, programa);

    pid_t pid = fork();

    if (pid < 0) {
        throw std::invalid_argument("poti$h erro: Falha no (fork) " + programa);
    } else if (pid == 0) {
        std::vector<char*> argv = converterParaArgv(args);
        execve(absolute_path.c_str(), argv.data(), environ);
        exit(1);
    } else {
        waitpid(pid, nullptr, 0);
    }
}
