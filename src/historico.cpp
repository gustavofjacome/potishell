#include "historico.h"

#include <deque>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <limits.h>
#include <unistd.h>

// process_command é definida em potish.cpp; o offset do history reutiliza o pipeline normal de execução
void process_command(std::string command);

namespace Historico {
    std::deque<std::string> emMemoria; // cache em RAM; o arquivo .potish_historico é o "backup" persistente

    // descobre a pasta onde o executavel do potish esta rodando (nao a pasta atual do terminal)
    // assim o arquivo de historico sempre fica junto do binario, independente de onde o usuario chamou o shell
    std::string obterCaminhoArquivo() {
        char buffer[PATH_MAX];
        ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
        std::string pastaExecutavel = "."; // fallback caso o readlink falhe

        if (len != -1) {
            buffer[len] = '\0';
            std::string caminhoCompleto(buffer);
            size_t pos = caminhoCompleto.find_last_of('/');
            if (pos != std::string::npos) {
                pastaExecutavel = caminhoCompleto.substr(0, pos);
            }
        }
        return pastaExecutavel + "/.potish_historico";
    }

    // chamada uma unica vez, no inicio do main(), pra popular a memoria com o que ja foi salvo em execucoes anteriores
    void carregar() {
        std::ifstream arquivo(obterCaminhoArquivo());
        if (!arquivo.is_open()) {
            return; // primeira vez rodando o shell, arquivo ainda nem existe
        }

        std::string linha;
        while (std::getline(arquivo, linha)) {
            if (!linha.empty()) {
                // o arquivo guarda do mais antigo (topo) pro mais novo (fim)
                // dar push_front na ordem de leitura deixa o comando mais recente na frente do deque
                emMemoria.push_front(linha);
            }
        }
    }

    void adicionar(const std::string& comando) {
        emMemoria.push_front(comando);

        // ios::app so acrescenta a linha no final do arquivo, sem reescrever o historico inteiro
        std::ofstream arquivo(obterCaminhoArquivo(), std::ios::app);
        if (arquivo.is_open()) {
            arquivo << comando << '\n';
        }
    }

    void imprimir() {
        for (int i = emMemoria.size() - 1; i >= 0; --i) {
            std::cout << i << " " << emMemoria[i] << '\n';
        }
    }

    void limpar() {
        emMemoria.clear();
        // abrir em modo trunc e fechar na hora ja apaga todo o conteudo do arquivo
        std::ofstream arquivo(obterCaminhoArquivo(), std::ios::trunc);
    }

    void executarPorOffset(int offset) { 
        if (offset < 0 || (offset + 1) >= (int)emMemoria.size()) {
            throw std::runtime_error("poti$h erro: Offset de history inválido.");
        }
        std::string cmdSalvo = emMemoria[offset + 1];
        std::cout << cmdSalvo << '\n'; 
        process_command(cmdSalvo);    
    }
}
