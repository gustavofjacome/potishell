#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdexcept>
#include <sys/stat.h>
#include <stdio.h>
#include <algorithm>
#include <string>
#include <vector>


//design
namespace Cor {
    const std::string RESET   = "\033[0m";
    const std::string VERMELHO= "\033[1;31m";
    const std::string VERDE   = "\033[1;32m";
    const std::string AMARELO = "\033[1;33m";
    const std::string AZUL    = "\033[1;34m";
    const std::string CIANO   = "\033[1;36m";

    inline std::string vermelho(const std::string& texto) { 
        return VERMELHO + texto + RESET; 
    }
    inline std::string verde(const std::string& texto)    {
         return VERDE + texto + RESET; 
    }
    inline std::string amarelo(const std::string& texto)  { 
        return AMARELO + texto + RESET; 
    }
    inline std::string azul(const std::string& texto)     { 
        return AZUL + texto + RESET; 
    }
    inline std::string ciano(const std::string& texto)    {
         return CIANO + texto + RESET; 
    }
}









void comandosInternos(std::string comandoInterno) {

}










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



void verificacoesErros(const std::string& absolute_path, const std::string& command) {
    if (access(absolute_path.c_str(), F_OK) != 0) {
        throw std::runtime_error("poti$h erro: Comando não encontrado: " + command);
    }

    if (access(absolute_path.c_str(), X_OK) != 0) {
        throw std::runtime_error("poti$h erro: Sem permissão para executar: " + command);
    }
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
    


    std::vector<std::string> args = geraVetorAgumentos(command);
    if (args.empty()) {
        return;
    } 

    std::string programa = args[0];
    std::string absolute_path = "/bin/" + programa;

    if (std::count(command.begin(), command.end(), '/') > 0) {
        absolute_path = command;
    }

    verificacoesErros(absolute_path, command);


    pid_t pid = fork();

    if (pid < 0) {
        throw std::invalid_argument("poti$h erro: Falha no (fork) " + command);
        return;

    } else if (pid == 0) {
        std::vector<char*> argv = converterParaArgv(args);
            execve(absolute_path.c_str(), argv.data(), NULL);
            exit(1);
    } else {
        throw std::runtime_error("erro: Sem permissão para executar: " + command);
    }
}

int main() {
    const std::string potishFormatado = Cor::amarelo("poti$h🦐 ") + Cor::ciano("❯ ");



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
