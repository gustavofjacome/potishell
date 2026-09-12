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

namespace Terminal {
    const std::string LIMPAR_TELA = "\033[2J\033[1;1H"; 
    const std::string LIMPAR_LINHA = "\033[2K";         
    
    inline void limpar() {
        std::cout << LIMPAR_TELA;
    }
}

// defini os cabeçalhos das funções aqui em cima apenas para permitir deixar a main como primeira função
std::vector<std::string> geraVetorAgumentos(std::string comando);
std::vector<char*> converterParaArgv(std::vector<std::string>& args);
void verificacoesErros(const std::string& absolute_path, const std::string& programa);
bool executarComandosInternos(const std::vector<std::string>& args);
void executarComandosExterno(std::vector<std::string>& args);
void process_command(std::string command);



//main
int main() {
    const std::string nomeBashFormatado = Cor::amarelo("poti$h🦐 ") + Cor::ciano("❯ ");
    potishLoop(true, nomeBashFormatado);
    return 0;
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

void verificacoesErros(const std::string& absolute_path, const std::string& programa) {
    if (access(absolute_path.c_str(), F_OK) != 0) {
        throw std::runtime_error("poti$h erro: Comando não encontrado: " + programa);
    }
    if (access(absolute_path.c_str(), X_OK) != 0) {
        throw std::runtime_error("poti$h erro: Sem permissão para executar: " + programa);
    }
}


bool executarComandosInternos(const std::vector<std::string>& args) {
    if (args.empty()) {
        return false;
    }
    
    std::string comando = args[0];

    if (comando == "sair" || comando == "exit") {
        exit(0);
        return true;
    }

    if (comando == "ajuda" || comando == "help") {
        std::cout << "poti$h🦐 - Comandos internos disponíveis:\n";
        std::cout << "  cd [dir]   Muda de diretório\n";
        std::cout << "  clear      Limpa a tela\n";
        std::cout << "  exit       Sai do shell\n";
        return true;
    }

    if (comando == "potish" && args.size() > 1) {
        if (args[1] == "--versao" || args[1] == "-versao" || args[1] == "-v") {
            std::cout << "1.1" << '\n';
            return true;
        }
    }

    if (comando == "limpar" || comando == "clear" || comando == "cls") {
        Terminal::limpar();
        return true;
    }

    if (comando == "cd") {
        if (args.size() < 2) {
            throw std::runtime_error("poti$h erro: 'cd' precisa de um diretório alvo.");
        } else {
            if (chdir(args[1].c_str()) != 0) {
                throw std::runtime_error("poti$h erro: Diretório não encontrado: " + args[1]);
            }
        }
        return true;
    }
    
    return false; // se nao caiu em nenhum é interno
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
        execve(absolute_path.c_str(), argv.data(), NULL);
        exit(1);
    } else {
        waitpid(pid, nullptr, 0);
    }
}


void process_command(std::string command) {
    
    std::vector<std::string> args = geraVetorAgumentos(command);
    if (args.empty()) {
        return; 
    } 

    if (executarComandosInternos(args)) {
        return; 
    } else {
        executarComandosExterno(args);
    }

    
}


void potishLoop(bool interruptor, std::string nomeShell){
    while (interruptor) {
        std::cout << nomeShell;
        std::string command;
        getline(std::cin, command);

        if (!command.empty()) {
            try {
                process_command(command);
            }
            catch (const std::exception& e) {
                std::cout << Cor::vermelho(e.what()) << "\n";
            }
        }
    }
}

