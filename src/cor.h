#pragma once

#include <string>

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
