#pragma once

#include <string>

namespace Historico {
    void carregar();
    void adicionar(const std::string& comando);
    void imprimir();
    void limpar();
    void executarPorOffset(int offset);
}
