#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdlib>
#include <unistd.h>

#include "cor.h"
#include "sclide.h"
#include "historico.h"
#include "utils.h"

bool executarComandosInternos(const std::vector<std::string>& args);
void process_command(std::string command);
void potishLoop(bool interruptor, std::string nomeShell);

int main() {
    Historico::carregar();
    const std::string nomeBashFormatado = Cor::amarelo("poti$h🦐 ") + Cor::ciano("❯ ");
    potishLoop(true, nomeBashFormatado);
    return 0;
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
        std::cout << '\n';
        std::cout << "  " << Cor::amarelo("poti$h🦐 ") << "- Comandos Internos Disponíveis (PT/EN)\n";
        std::cout << "  ------------------------------------------------------------\n";
        std::cout << "  " << Cor::ciano("ajuda, help") << "           Exibe este menu de ajuda\n";
        std::cout << "  " << Cor::ciano("cd ") << Cor::verde("[dir]") << "              Muda o diretório atual de trabalho\n";
        std::cout << "  " << Cor::ciano("historico, history") << "    Lista os últimos 10 comandos digitados\n";
        std::cout << "  " << Cor::ciano("historico ") << Cor::verde("-c") << "         Limpa todo o histórico da sessão\n";
        std::cout << "  " << Cor::ciano("historico ") << Cor::verde("[0-9]") << "      Executa um comando salvo pelo seu índice\n";
        std::cout << "  " << Cor::ciano("limpar, clear, cls") << "    Limpa a tela do terminal\n";
        std::cout << "  " << Cor::ciano("potish ") << Cor::verde("-v, --versao") << "   Exibe a versão atual do shell\n";
        std::cout << "  " << Cor::ciano("pwd") << "                   Exibe o caminho do diretório atual\n";
        std::cout << "  " << Cor::ciano("sair, exit") << "            Encerra o shell\n";
        std::cout << '\n';
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
    if (comando == "pwd") {
        char buffer[2048];
        if (getcwd(buffer, sizeof(buffer)) != nullptr) {
            std::cout << buffer << '\n';
        } else {
            throw std::runtime_error("poti$h erro: Falha ao obter o diretório atual.");
        }
        return true;
        
    }
    if (comando == "history" || comando == "historico") {
        if (args.size() == 1) {
            Historico::imprimir();
            return true;
        } 
        else if (args.size() == 2) {
            if (args[1] == "-c") {
                Historico::limpar();
                return true;
            } 
            else {
                try {
                    int offset = std::stoi(args[1]); 
                    Historico::executarPorOffset(offset); 
                    return true;
                } catch (const std::invalid_argument&) {
                    throw std::runtime_error("poti$h erro: history offset deve ser um número válido. (1 à 10)");
                }
            }
        }
    }
    return false; // se nao caiu em nenhum é interno
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
            Historico::adicionar(command);
            try {
                process_command(command);
            }
            catch (const std::exception& erro) {
                std::cout << Cor::vermelho(erro.what()) << "\n";
            }
        }
    }
}
